#include "particle_manager.h"
#include "particle_memory.h"

#include "engine/application.h"
#include "tinyxml2/tinyxml2.h"

#include <algorithm>

#define MAX_PARTICLE	8192

namespace particle
{
	typedef std::map<std::string, std::string> PropMap;

	static auto vec2ToString = [](const Vec2& vec)
	{
		std::string val;
		val.append(std::to_string(vec.x)).append(",").append(std::to_string(vec.y));
		return val;
	};

	static auto color4ToString = [](const Color& c)
	{
		std::string val; 
		val.append(std::to_string(c.r)).append(",")
			.append(std::to_string(c.g)).append(",")
			.append(std::to_string(c.b)).append(",")
			.append(std::to_string(c.a));
		return val;
	};

	static auto getString = [](const PropMap& props, const std::string& name)
	{
		auto it = props.find(name);
		return it == props.end() ? "" : it->second;
	};

	static auto getInt = [](const PropMap& props, const std::string& name)
	{
		auto it = props.find(name);
		return it == props.end() ? 0 : std::stoi(it->second);
	};

	static auto getFloat = [](const PropMap& props, const std::string& name)
	{
		auto it = props.find(name);
		return it == props.end() ? 0.0f : std::stof(it->second);
	};

	static auto getBool = [](const PropMap& props, const std::string& name)
	{
		auto it = props.find(name);
		return it == props.end() ? false : std::stoi(it->second) != 0;
	};

	static auto getVec2 = [](const PropMap& props, const std::string& name)
	{
		auto it = props.find(name);
		if (it == props.end())
		{
			return Vec2(0.0f, 0.0f);
		}
		else
		{
			float x, y; sscanf_s(it->second.c_str(), "%f,%f", &x, &y);
			return Vec2(x, y);
		}
	};

	static auto getColor4f = [](const PropMap& props, const std::string& name)
	{
		auto it = props.find(name);
		if (it == props.end())
		{
			return Color();
		}
		else
		{
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


	void Particle::setTexture(const std::string& file)

	{
		auto& textureMgr = ParticleManager::inst().getApplication()->resourceManager().textureManager();

		_texture = textureMgr.get(HashString(file.c_str()));
		if (!_texture)
		{
			LogError("Particle::setTexture: {} failed. texture not found.", file);
			return;
		}
	}

	void Particle::load(const std::string& filename)
	{
		_file = filename;

		auto fullpath = ParticleManager::inst().getApplication()->resPath() / filename;
		
		ParticleDescription&& desc = createParticleDescription(fullpath.string());
		setDescription(desc);
	}

	void Particle::setDescription(const ParticleDescription& desc)
	{
		setTexture(desc.texture);
		_localEmitPos = desc.emitPos;

		if (_emitter)
		{
			_emitter->setDescription(desc);
		}

		_vertexData.resize(desc.particleCount * 4);
		_indices.resize(desc.particleCount * 6);

		for (int i = 0; i < desc.particleCount; i++)
		{
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

	void Particle::update(float dt)
	{
		if (_emitter)
		{
			_emitter->update(dt);
		}
	}

	void Particle::draw(engine::Camera* camera, bool screenSpace)
	{
		auto particleList = _emitter->getParticleList();
		if ( particleList->empty() ) return;

		float s = 0, c = 0, x = 0, y = 0;
		int vertexIndex = 0, indexIndex = 0, i = 0;

		for ( auto& particle : *particleList)
		{
			assert(particle);
			fail_continue(particle);

			float hsize = particle->size / 2.0f;
			c = cosf(particle->rotation) * hsize;
			s = sinf(particle->rotation) * hsize;

			x = particle->pos.x;
			y = particle->pos.y;

			Vec2 p1 = {x - c - s, y - c + s};
			Vec2 p2 = {x - c + s, y + c + s};
			Vec2 p3 = {x + c + s, y + c - s};
			Vec2 p4 = {x + c - s, y - c - s};

			if(camera)
			{
				if (screenSpace)
				{
					p1 = camera->projectScreenPoint(p1);
					p2 = camera->projectScreenPoint(p2);
					p3 = camera->projectScreenPoint(p3);
					p4 = camera->projectScreenPoint(p4);
				}
				else
				{
					p1 = camera->projectPoint(p1);
					p2 = camera->projectPoint(p2);
					p3 = camera->projectPoint(p3);
					p4 = camera->projectPoint(p4);
				}
			}

			_vertexData[i * 4 + 0].position = ToPoint(p1);
			_vertexData[i * 4 + 1].position = ToPoint(p2);
			_vertexData[i * 4 + 2].position = ToPoint(p3);
			_vertexData[i * 4 + 3].position = ToPoint(p4);

			Color color = particle->color;
			color.a = static_cast<uint8_t>(std::clamp(color.a * _alphaScale, 0.0f, 255.0f));
			_vertexData[i * 4 + 0].color = color;
			_vertexData[i * 4 + 1].color = color;
			_vertexData[i * 4 + 2].color = color;
			_vertexData[i * 4 + 3].color = color;

			i++;
		}

		int vex_count = particleList->size() * 4;
		int idx_count = particleList->size() * 6;

		auto& renderer = ParticleManager::inst().getApplication()->renderer();
		renderer.drawGeometry(_texture, _vertexData.data(), _vertexData.size(), _indices.data(), _indices.size());
	}

	ParticleDescription Particle::createParticleDescription(const std::string& filename)
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
		while (secEle)
		{
			XMLElement* propEle = secEle->FirstChildElement("prop");
			while (propEle)
			{
				std::string propName = propEle->Attribute("name");
				std::string propValue = propEle->Attribute("value");
				props.insert({propName, propValue});

				propEle = propEle->NextSiblingElement("prop");
			}

			secEle = secEle->NextSiblingElement();
		}

		// desc
		desc.texture = getString(props, "texture_file");

		// emmiter
		desc.emitPos = getVec2(props, "start_pos");
		desc.emitPosVar = getVec2(props, "start_pos_variance");

		desc.emitterShape = (EmitterShape)getInt(props, "emitter_shape");
		desc.emitShapeSize = getVec2(props, "emitter_shape_size");
		desc.emitShapeRadius = getFloat(props, "emitter_shape_radius");
		desc.emitShapeRotation = getFloat(props, "emitter_shape_rotation");
		desc.emitShapeEdgeOnly = getBool(props, "emitter_shape_edge_only");

		if (props.find("emitter_shape") == props.end())
		{
			desc.emitterShape = EmitterShape::Rect;
		}

		if (props.find("emitter_shape_size") == props.end())
		{
			desc.emitShapeSize = desc.emitPosVar * 2.0f;
		}

		desc.emitAngle = getFloat(props, "angle");
		desc.emitAngleVar = getFloat(props, "angle_variance");

		desc.emitSpeed = getFloat(props, "speed");
		desc.emitSpeedVar = getFloat(props, "speed_variance");

		desc.emitterType = (EmitterType)getInt(props, "emit_type");
		desc.duration = getFloat(props, "duration");

		desc.particleCount = getInt(props, "max_particles");

		desc.life = getFloat(props, "life_span");
		desc.lifeVar = getFloat(props, "life_span_variance");


		// effect
		desc.beginColor = getColor4f(props, "start_color");
		desc.beginColorVar = getColor4f(props, "start_color_variance");

		desc.endColor = getColor4f(props, "finish_color");
		desc.endColorVar = getColor4f(props, "finish_color_variance");

		desc.beginSize = getFloat(props, "start_size");
		desc.beginSizeVar = getFloat(props, "start_size_variance");

		desc.endSize = getFloat(props, "finish_size");
		desc.endSizeVar = getFloat(props, "finish_size_variance");

		desc.beginSpin = getFloat(props, "start_rotation");
		desc.beginSpinVar = getFloat(props, "start_rotation_variance");

		desc.endSpin = getFloat(props, "finish_rotation");
		desc.endSpinVar = getFloat(props, "finish_rotation_variance");

		desc.motionMode = (MotionMode)getInt(props, "motion_mode");

		desc.gravityMode.gravity = getVec2(props, "gravity");
		desc.gravityMode.radialAccel = getFloat(props, "radial_acc");
		desc.gravityMode.radialAccelVar = getFloat(props, "radial_acc_variance");
		desc.gravityMode.tangentialAccel = getFloat(props, "tangent_acc");
		desc.gravityMode.tangentialAccelVar = getFloat(props, "tangent_acc_variance");
		desc.radiusMode.beginRadius = getFloat(props, "min_radius");
		desc.radiusMode.beginRadiusVar = getFloat(props, "min_radius_variance");
		desc.radiusMode.endRadius = getFloat(props, "max_radius");
		desc.radiusMode.endRadiusVar = getFloat(props, "max_radius_variance");
		desc.radiusMode.spinPerSecond = getFloat(props, "rotate_speed");
		desc.radiusMode.spinPerSecondVar = getFloat(props, "rotate_speed_variance");

		correctDescription(desc);

		return desc;
	}

	// 修正一些数据
	void Particle::correctDescription(ParticleDescription& desc)
	{
		desc.life = FloatEqual(desc.life, 0) ? 0.1 : desc.life;
		desc.emitRate = desc.particleCount / desc.life;
	}

	Vec2 Particle::getPos()
	{
		if (_emitter)
		{
			Vec2&& pos = _emitter->getEmitPos();
			return { (int)pos.x, (int)pos.y};
		}

		return {0,0};
	}

	void Particle::setPos(const Vec2& pos)
	{
		if (_emitter)
		{
			_emitter->setEmitPos({(float)pos.x, (float)pos.y});
		}
	}

	void Particle::setOwnerPos(const Vec2& pos)
	{
		if (_emitter)
		{
			_emitter->setEmitPos(_localEmitPos + pos);
		}
	}

	void Particle::start()
	{
		if (_emitter)
		{
			_emitter->startEmitting();
		}
	}
	
	void Particle::stop()
	{
		if (_emitter)
		{
			_emitter->stopEmitting();
		}
	}

	void Particle::setAlphaScale(float alpha)
	{
		_alphaScale = std::clamp(alpha, 0.0f, 1.0f);
	}

	float Particle::getAlphaScale() const
	{
		return _alphaScale;
	}

	bool Particle::isPlaying()
	{
		return _emitter && _emitter->isEmitting();
	}

	std::string Particle::getName()
	{
		return _name;
	}

	std::string Particle::getFile()
	{
		return _file;
	}

	ParticleEffect* Particle::getEffect()
	{
		return _emitter->getParticleEffect();
	}

	ParticleEmitter* Particle::getEmitter()
	{
		return _emitter.get();
	}

#ifdef Particle_Editor_Mode

	ParticleDescription& Particle::description()
	{
		return _descript;
	}

	void Particle::reset()
	{
		correctDescription(_descript);

		setDescription(_descript);
	}

	void Particle::save()
	{
		using namespace tinyxml2;
		auto fullpath = ParticleManager::inst().getApplication()->resPath() / _file;
		
		tinyxml2::XMLDocument doc;
		XMLError error = doc.LoadFile(fullpath.string().c_str());
		if (error != XML_SUCCESS)
		{
			LogError("Particle::save: open file {} failed. err = {}", fullpath.string(), (int)error);
			return;
		}

		PropMap props;
		props["texture_file"] = _descript.texture;
		props["start_pos"] = vec2ToString(_descript.emitPos);
		props["start_pos_variance"] = vec2ToString(_descript.emitPosVar);
		props["emitter_shape"] = std::to_string((int)_descript.emitterShape);
		props["emitter_shape_size"] = vec2ToString(_descript.emitShapeSize);
		props["emitter_shape_radius"] = std::to_string(_descript.emitShapeRadius);
		props["emitter_shape_rotation"] = std::to_string(_descript.emitShapeRotation);
		props["emitter_shape_edge_only"] = _descript.emitShapeEdgeOnly ? "1" : "0";

		props["angle"] = std::to_string(_descript.emitAngle);
		props["angle_variance"] = std::to_string(_descript.emitAngleVar);
		props["speed"] = std::to_string(_descript.emitSpeed);
		props["speed_variance"] = std::to_string(_descript.emitSpeedVar);

		props["emit_type"] = std::to_string((int)_descript.emitterType);
		props["duration"] = std::to_string(_descript.duration);
		props["max_particles"] = std::to_string(_descript.particleCount);

		props["life_span"] = std::to_string(_descript.life);
		props["life_span_variance"] = std::to_string(_descript.lifeVar);

		props["start_color"] = color4ToString(_descript.beginColor);
		props["start_color_variance"] = color4ToString(_descript.beginColorVar);
		props["finish_color"] = color4ToString(_descript.endColor);
		props["finish_color_variance"] = color4ToString(_descript.endColorVar);

		props["start_size"] = std::to_string(_descript.beginSize);
		props["start_size_variance"] = std::to_string(_descript.beginSizeVar);
		props["finish_size"] = std::to_string(_descript.endSize);
		props["finish_size_variance"] = std::to_string(_descript.endSizeVar);

		props["start_rotation"] = std::to_string(_descript.beginSpin);
		props["start_rotation_variance"] = std::to_string(_descript.beginSpinVar);

		props["finish_rotation"] = std::to_string(_descript.endSpin);
		props["finish_rotation_variance"] = std::to_string(_descript.endSpinVar);

		props["motion_mode"] = std::to_string((int)_descript.motionMode);

		props["gravity"] = vec2ToString(_descript.gravityMode.gravity);
		props["radial_acc"] = std::to_string(_descript.gravityMode.radialAccel);
		props["radial_acc_variance"] = std::to_string(_descript.gravityMode.radialAccelVar);
		props["tangent_acc"] = std::to_string(_descript.gravityMode.tangentialAccel);
		props["tangent_acc_variance"] = std::to_string(_descript.gravityMode.tangentialAccelVar);

		props["min_radius"] = std::to_string(_descript.radiusMode.beginRadius);
		props["min_radius_variance"] = std::to_string(_descript.radiusMode.beginRadiusVar);
		props["max_radius"] = std::to_string(_descript.radiusMode.endRadius);
		props["max_radius_variance"] = std::to_string(_descript.radiusMode.endRadiusVar);
		props["rotate_speed"] = std::to_string(_descript.radiusMode.spinPerSecond);
		props["rotate_speed_variance"] = std::to_string(_descript.radiusMode.spinPerSecondVar);


		XMLElement* root = doc.RootElement();
		XMLElement* secEle = root->FirstChildElement();
		while (secEle)
		{
			XMLElement* propEle = secEle->FirstChildElement("prop");
			while (propEle)
			{
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

	ParticlePtr ParticleManager::createParticle(const std::string& name)
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

			pParticle->load(file);
			pParticle->stop();
		}

		return pParticle;
	}

	const StringStringMap& ParticleManager::getAllParticleConfigs()
	{
		return _particleFiles;
	}

	engine::Application* ParticleManager::getApplication()
	{
		return _application;
	}


	bool ParticleManager::reload()

	{
		if(_cfgDir.empty()) return false;

		return loadParticles(_cfgDir);
	}
	
	bool ParticleManager::loadParticles(const std::filesystem::path& cfgdir)
	{
		_cfgDir = cfgdir;
		_particleFiles.clear();

		for (const auto& entry : std::filesystem::directory_iterator(cfgdir))
		{
			if (entry.is_regular_file() && entry.path().extension() == ".xml")
			{
				auto filename = entry.path();

				_particleFiles.insert({filename.stem().string(), filename.string() });
			}
		}

		return true;
	}
}
