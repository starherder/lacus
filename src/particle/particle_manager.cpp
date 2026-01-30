#include "particle_manager.h"
#include "particle_memory.h"

#include "engine/application.h"
#include "tinyxml2/tinyxml2.h"

#define MAX_PARTICLE	8192

namespace particle
{
	typedef std::map<std::string, std::string> PropMap;

	static auto vec2ToString = [](const Vec2& vec) {
		std::string val;
		val.append(std::to_string(vec.x)).append(",").append(std::to_string(vec.y));
		return val;
	};

	static auto color4ToString = [](const Color& c) {
		std::string val; 
		val.append(std::to_string(c.r)).append(",")
			.append(std::to_string(c.g)).append(",")
			.append(std::to_string(c.b)).append(",")
			.append(std::to_string(c.a));
		return val;
	};

	static auto getString = [](const PropMap& props, const std::string& name) {
		auto it = props.find(name);
		return it == props.end() ? "" : it->second;
	};

	static auto getInt = [](const PropMap& props, const std::string& name) {
		auto it = props.find(name);
		return it == props.end() ? 0 : std::stoi(it->second);
	};

	static auto getFloat = [](const PropMap& props, const std::string& name) {
		auto it = props.find(name);
		return it == props.end() ? 0.0f : std::stof(it->second);
	};

	static auto getVec2 = [](const PropMap& props, const std::string& name) {
		auto it = props.find(name);
		if (it == props.end()) {
			return Vec2(0.0f, 0.0f);
		}
		else {
			float x, y; sscanf_s(it->second.c_str(), "%f,%f", &x, &y);
			return Vec2(x, y);
		}
	};

	static auto getColor4f = [](const PropMap& props, const std::string& name) {
		auto it = props.find(name);
		if (it == props.end()) {
			return Color();
		}
		else {
			int r, g, b, a; sscanf_s(it->second.c_str(), "%d,%d,%d,%d", &r, &g, &b, &a);
			return Color(r, g, b, a);
		}
	};

	Particle::Particle(const std::string& name)
	{
		_name = name;
		_emitter = std::make_unique<ParticleEmitter>();

		//LogInfo("Particle({}) construct.", _name);
	}

	Particle::~Particle()
	{
		//LogInfo("Particle({}) destruct.", _name);
	}


	void Particle::SetTexture(const std::string& file)
	{
		auto& textureMgr = ParticleManager::inst().GetApplication()->resourceManager().textureManager();

		m_Texture = textureMgr.get(HashString(file.c_str()));
		if (!m_Texture)
		{
			LogError("Particle::SetTexture: {} failed. texture not found.", file);
			return;
		}
	}

	void Particle::Load(const std::string& filename)
	{
		_file = filename;

		auto fullpath = ParticleManager::inst().GetApplication()->resPath() / filename;
		
		ParticleDescription&& desc = CreateParticleDescription(fullpath.string());
		SetDescription(desc);
	}

	void Particle::SetDescription(const ParticleDescription& desc)
	{
		SetTexture(desc.sTexture);

		if (_emitter) {
			_emitter->setDecription(desc);
		}

		_vertexData.resize(desc.nParticleCount * 4);
		_indices.resize(desc.nParticleCount * 6);

		for (int i = 0; i < desc.nParticleCount; i++) {
			_vertexData[i * 4 + 0].tex_coord = { 0, 0 };
			_vertexData[i * 4 + 1].tex_coord = { 0, 1 };
			_vertexData[i * 4 + 2].tex_coord = { 1, 1 };
			_vertexData[i * 4 + 3].tex_coord = { 1, 0 };

			_indices[i * 6 + 0] = i * 4 + 0;
			_indices[i * 6 + 1] = i * 4 + 2;
			_indices[i * 6 + 2] = i * 4 + 1;
			_indices[i * 6 + 3] = i * 4 + 0;
			_indices[i * 6 + 4] = i * 4 + 3;
			_indices[i * 6 + 5] = i * 4 + 2;
		}

#ifdef Particle_Editor_Mode 
		_descript = desc;
#endif
	}

	void Particle::Update(float dt)
	{
		if (_emitter) {
			_emitter->update(dt);
		}
	}

	void Particle::Draw(engine::Camera* camera)
	{
		auto particleList = _emitter->getParticleList();
		if ( particleList->empty() ) return;

		float s = 0, c = 0, x = 0, y = 0;
		int vertexIndex = 0, indexIndex = 0, i = 0;

		for ( auto& particle : *particleList) {
			assert(particle);
			fail_continue(particle);

			float hsize = particle->fSize / 2.0f;
			c = cosf(particle->fRotation) * hsize;
			s = sinf(particle->fRotation) * hsize;

			x = particle->vPos.x;
			y = particle->vPos.y;

			Vec2 p1 = {x - c - s, y - c + s};
			Vec2 p2 = {x - c + s, y + c + s};
			Vec2 p3 = {x + c + s, y + c - s};
			Vec2 p4 = {x + c - s, y - c - s};

			if(camera) 
			{
				p1 = camera->projectPoint(p1);
				p2 = camera->projectPoint(p2);
				p3 = camera->projectPoint(p3);
				p4 = camera->projectPoint(p4);
			}

			_vertexData[i * 4 + 0].position = ToPoint(p1);
			_vertexData[i * 4 + 1].position = ToPoint(p2);
			_vertexData[i * 4 + 2].position = ToPoint(p3);
			_vertexData[i * 4 + 3].position = ToPoint(p4);

			_vertexData[i * 4 + 0].color = particle->cColor;
			_vertexData[i * 4 + 1].color = particle->cColor;
			_vertexData[i * 4 + 2].color = particle->cColor;
			_vertexData[i * 4 + 3].color = particle->cColor;

			i++;
		}

		int vex_count = particleList->size() * 4;
		int idx_count = particleList->size() * 6;

		auto& renderer = ParticleManager::inst().GetApplication()->renderer();
		renderer.drawGeometry(m_Texture, _vertexData.data(), _vertexData.size(), _indices.data(), _indices.size());
	}

	ParticleDescription Particle::CreateParticleDescription(const std::string& filename)
	{
		using namespace tinyxml2;

		PropMap props;
		ParticleDescription desc;

		tinyxml2::XMLDocument doc;
		XMLError error = doc.LoadFile(filename.c_str());
		if (error!= XML_SUCCESS)
		{
			return desc;
		}

		XMLElement* root = doc.RootElement();
		XMLElement* secEle = root->FirstChildElement();
		while (secEle) {
			XMLElement* propEle = secEle->FirstChildElement("prop");
			while (propEle) {
				std::string propName = propEle->Attribute("name");
				std::string propValue = propEle->Attribute("value");
				props.insert({propName, propValue});

				propEle = propEle->NextSiblingElement("prop");
			}

			secEle = secEle->NextSiblingElement();
		}

		// desc
		desc.sTexture = getString(props, "texture_file");

		// emmiter
		desc.vEmitPos = getVec2(props, "start_pos");
		desc.vEmitPosVar = getVec2(props, "start_pos_variance");

		desc.fEmitAngle = getFloat(props, "angle");
		desc.fEmitAngleVar = getFloat(props, "angle_variance");

		desc.fEmitSpeed = getFloat(props, "speed");
		desc.fEmitSpeedVar = getFloat(props, "speed_variance");

		desc.emitterType = (EmitterType)getInt(props, "emit_type");
		desc.fDuration = getFloat(props, "duration");

		desc.nParticleCount = getInt(props, "max_particles");

		desc.fLife = getFloat(props, "life_span");
		desc.fLifeVar = getFloat(props, "life_span_variance");


		// effect
		desc.cBeginColor = getColor4f(props, "start_color");
		desc.cBeginColorVar = getColor4f(props, "start_color_variance");

		desc.cEndColor = getColor4f(props, "finish_color");
		desc.cBeginColorVar = getColor4f(props, "finish_color_variance");

		desc.fBeginSize = getFloat(props, "start_size");
		desc.fBeginSizeVar = getFloat(props, "start_size_variance");

		desc.fEndSize = getFloat(props, "finish_size");
		desc.fEndSizeVar = getFloat(props, "finish_size_variance");

		desc.fBeginSpin = getFloat(props, "start_rotation");
		desc.fBeginSpinVar = getFloat(props, "start_rotation_variance");

		desc.fEndSpin = getFloat(props, "finish_rotation");
		desc.fEndSpinVar = getFloat(props, "finish_rotation_variance");

		desc.motionMode = (MotionMode)getInt(props, "motion_mode");

		desc.gravityMode.vGravity = getVec2(props, "gravity");
		desc.gravityMode.fRadialAccel = getFloat(props, "radial_acc");
		desc.gravityMode.fRadialAccelVar = getFloat(props, "radial_acc_variance");
		desc.gravityMode.fTangentialAccel = getFloat(props, "tangent_acc");
		desc.gravityMode.fTangentialAccelVar = getFloat(props, "tangent_acc_variance");
		desc.radiusMode.fBeginRadius = getFloat(props, "min_radius");
		desc.radiusMode.fBeginRadiusVar = getFloat(props, "min_radius_variance");
		desc.radiusMode.fEndRadius = getFloat(props, "max_radius");
		desc.radiusMode.fEndRadiusVar = getFloat(props, "max_radius_variance");
		desc.radiusMode.fSpinPerSecond = getFloat(props, "rotate_speed");
		desc.radiusMode.fSpinPerSecondVar = getFloat(props, "rotate_speed_variance");

		CorrectDescription(desc);

		return desc;
	}

	// 修正一些数据
	void Particle::CorrectDescription(ParticleDescription& desc)
	{
		desc.fLife = FloatEqual(desc.fLife, 0) ? 0.1 : desc.fLife;
		desc.fEmitRate = desc.nParticleCount / desc.fLife;
	}

	Vec2 Particle::GetPos()
	{
		if (_emitter)
		{
			Vec2&& pos = _emitter->GetEmitPos();
			return { (int)pos.x, (int)pos.y};
		}

		return {0,0};
	}

	void Particle::SetPos(const Vec2& pos)
	{
		if (_emitter)
		{
			_emitter->SetEmitPos({(float)pos.x, (float)pos.y});
		}
	}

	void Particle::Start()
	{
		if (_emitter)
		{
			_emitter->startEmitting();
		}
	}
	
	void Particle::Stop()
	{
		if (_emitter)
		{
			_emitter->stopEmitting();
		}
	}

	bool Particle::IsPlaying()
	{
		return _emitter && _emitter->isEmitting();
	}

	std::string Particle::GetName()
	{
		return _name;
	}

	std::string Particle::GetFile()
	{
		return _file;
	}

#ifdef Particle_Editor_Mode

	ParticleDescription& Particle::Description()
	{
		return _descript;
	}

	void Particle::Reset()
	{
		CorrectDescription(_descript);

		SetDescription(_descript);
	}

	void Particle::Save()
	{
		using namespace tinyxml2;
		auto fullpath = ParticleManager::inst().GetApplication()->resPath() / _file;
		
		tinyxml2::XMLDocument doc;
		XMLError error = doc.LoadFile(fullpath.string().c_str());
		if (error != XML_SUCCESS)
		{
			LogError("Particle::Save: open file {} failed. err = {}", fullpath.string(), (int)error);
			return;
		}

		PropMap props;
		props["texture_file"] = _descript.sTexture;
		props["start_pos"] = vec2ToString(_descript.vEmitPos);
		props["start_pos_variance"] = vec2ToString(_descript.vEmitPosVar);

		props["angle"] = std::to_string(_descript.fEmitAngle);
		props["angle_variance"] = std::to_string(_descript.fEmitAngleVar);
		props["speed"] = std::to_string(_descript.fEmitSpeed);
		props["speed_variance"] = std::to_string(_descript.fEmitSpeedVar);

		props["emit_type"] = std::to_string((int)_descript.emitterType);
		props["duration"] = std::to_string(_descript.fDuration);
		props["max_particles"] = std::to_string(_descript.nParticleCount);

		props["life_span"] = std::to_string(_descript.fLife);
		props["life_span_variance"] = std::to_string(_descript.fLifeVar);

		props["start_color"] = color4ToString(_descript.cBeginColor);
		props["start_color_variance"] = color4ToString(_descript.cBeginColorVar);
		props["finish_color"] = color4ToString(_descript.cEndColor);
		props["finish_color_variance"] = color4ToString(_descript.cEndColorVar);

		props["start_size"] = std::to_string(_descript.fBeginSize);
		props["start_size_variance"] = std::to_string(_descript.fBeginSizeVar);
		props["finish_size"] = std::to_string(_descript.fEndSize);
		props["finish_size_variance"] = std::to_string(_descript.fEndSizeVar);

		props["start_rotation"] = std::to_string(_descript.fBeginSpin);
		props["start_rotation_variance"] = std::to_string(_descript.fBeginSpinVar);

		props["finish_rotation"] = std::to_string(_descript.fEndSpin);
		props["finish_rotation_variance"] = std::to_string(_descript.fEndSpinVar);

		props["motion_mode"] = std::to_string((int)_descript.motionMode);

		props["gravity"] = vec2ToString(_descript.gravityMode.vGravity);
		props["radial_acc"] = std::to_string(_descript.gravityMode.fRadialAccel);
		props["radial_acc_variance"] = std::to_string(_descript.gravityMode.fRadialAccelVar);
		props["tangent_acc"] = std::to_string(_descript.gravityMode.fTangentialAccel);
		props["tangent_acc_variance"] = std::to_string(_descript.gravityMode.fTangentialAccelVar);

		props["min_radius"] = std::to_string(_descript.radiusMode.fBeginRadius);
		props["min_radius_variance"] = std::to_string(_descript.radiusMode.fBeginRadiusVar);
		props["max_radius"] = std::to_string(_descript.radiusMode.fEndRadius);
		props["max_radius_variance"] = std::to_string(_descript.radiusMode.fEndRadiusVar);
		props["rotate_speed"] = std::to_string(_descript.radiusMode.fSpinPerSecond);
		props["rotate_speed_variance"] = std::to_string(_descript.radiusMode.fSpinPerSecondVar);


		XMLElement* root = doc.RootElement();
		XMLElement* secEle = root->FirstChildElement();
		while (secEle) {
			XMLElement* propEle = secEle->FirstChildElement("prop");
			while (propEle) {
				std::string propName = propEle->Attribute("name");

				std::string propValue = props[propName];
				propEle->SetAttribute("value", propValue.c_str());
				
				propEle = propEle->NextSiblingElement("prop");
			}

			secEle = secEle->NextSiblingElement();
		}

		doc.SaveFile(fullpath.string().c_str());
	}

#endif
	//-------------------------------------------------------------------------------
	// 粒子系统管理器
	// ParticleSystemManager
	//-------------------------------------------------------------------------------
	ParticleManager::ParticleManager()
	{
		ParticleMemory::initParticleMemory(MAX_PARTICLE);
	}

	ParticleManager::~ParticleManager()
	{
		ParticleMemory::freeParticleMemory();
	}

	void ParticleManager::init(engine::Application* app)
	{
		_application = app;
	}

	ParticlePtr ParticleManager::CreateParticle(const std::string& name)
	{
		auto it = _particleFiles.find(name);
		if (it==_particleFiles.end())
		{
			return nullptr;
		}

		auto pParticle = std::make_shared<Particle>(name);
		if (pParticle)
		{
			const std::string& file = it->second;
			//LogInfo("create particle OK, name={}, file={}", name, file);

			pParticle->Load(file);
			pParticle->Stop();
		}

		return pParticle;
	}


	bool ParticleManager::Reload()
	{
		if(_cfgDir.empty()) return false;

		return LoadParticles(_cfgDir);
	}
	
	bool ParticleManager::LoadParticles(const std::filesystem::path& cfgdir)
	{
		_cfgDir = cfgdir;
		_particleFiles.clear();

		for (const auto& entry : std::filesystem::directory_iterator(cfgdir))
		{
			if (entry.is_regular_file())
			{
				auto filename = entry.path();

				_particleFiles.insert({filename.stem().string(), filename.string() });
			}
		}

		return true;
	}
}