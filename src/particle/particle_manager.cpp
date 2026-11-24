#include "particle_manager.h"
#include "particle_memory.h"

#include "engine/application.h"
#include "tinyxml2/tinyxml2.h"

#include <SDL3/SDL.h>
#include <algorithm>
#include <cmath>

#define MAX_PARTICLE	8192

namespace particle
{
	typedef std::map<std::string, std::string> PropMap;

	static float lengthSquared(const Vec2& vec)
	{
		return vec.x * vec.x + vec.y * vec.y;
	}

	static Vec2 safeNormal(const Vec2& vec)
	{
		float lenSqr = lengthSquared(vec);
		if (lenSqr <= 0.000001f)
		{
			return {0.0f, 0.0f};
		}
		return vec * (1.0f / std::sqrt(lenSqr));
	}

	static std::string vec2ToString(const Vec2& vec)
	{
		std::string val;
		val.append(std::to_string(vec.x)).append(",").append(std::to_string(vec.y));
		return val;
	}

	static std::string color4ToString(const Color& c)
	{
		std::string val; 
		val.append(std::to_string(c.r)).append(",")
			.append(std::to_string(c.g)).append(",")
			.append(std::to_string(c.b)).append(",")
			.append(std::to_string(c.a));
		return val;
	}

	static std::string getString(const PropMap& props, const std::string& name, const std::string& defaultValue = "")
	{
		auto it = props.find(name);
		return it == props.end() ? defaultValue : it->second;
	}

	static int getInt(const PropMap& props, const std::string& name, int defaultValue = 0)
	{
		auto it = props.find(name);
		if (it == props.end())
		{
			return defaultValue;
		}

		try
		{
			return std::stoi(it->second);
		}
		catch (const std::exception& e)
		{
			LogWarn("Particle config: parse int '{}'='{}' failed, err = '{}'.", name, it->second, e.what());
			return defaultValue;
		}
	}

	static float getFloat(const PropMap& props, const std::string& name, float defaultValue = 0.0f)
	{
		auto it = props.find(name);
		if (it == props.end())
		{
			return defaultValue;
		}

		try
		{
			return std::stof(it->second);
		}
		catch (const std::exception& e)
		{
			LogWarn("Particle config: parse float '{}'='{}' failed, err = '{}'.", name, it->second, e.what());
			return defaultValue;
		}
	}

	static bool getBool(const PropMap& props, const std::string& name, bool defaultValue = false)
	{
		auto it = props.find(name);
		if (it == props.end())
		{
			return defaultValue;
		}

		return getInt(props, name, defaultValue ? 1 : 0) != 0;
	}

	static Vec2 getVec2(const PropMap& props, const std::string& name, const Vec2& defaultValue = {0.0f, 0.0f})
	{
		auto it = props.find(name);
		if (it == props.end())
		{
			return defaultValue;
		}

		float x = 0.0f;
		float y = 0.0f;
		if (sscanf_s(it->second.c_str(), "%f,%f", &x, &y) != 2)
		{
			LogWarn("Particle config: parse vec2 '{}'='{}' failed.", name, it->second);
			return defaultValue;
		}

		return Vec2(x, y);
	}

	static Color getColor4f(const PropMap& props, const std::string& name, const Color& defaultValue = Color())
	{
		auto it = props.find(name);
		if (it == props.end())
		{
			return defaultValue;
		}

		int r = 0;
		int g = 0;
		int b = 0;
		int a = 0;
		if (sscanf_s(it->second.c_str(), "%d,%d,%d,%d", &r, &g, &b, &a) != 4)
		{
			LogWarn("Particle config: parse color '{}'='{}' failed.", name, it->second);
			return defaultValue;
		}

		return Color(r, g, b, a);
	}

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

		_texTile = textureMgr.getCfgTexTile(file);
		if (!_texTile || !_texTile->texture())
		{
			LogError("Particle::setTexture: {} failed. texture not found.", file);
			return;
		}
	}

	void Particle::load(const std::string& filename)
	{
		_file = filename;

		auto fullpath = ParticleManager::inst().getApplication()->resPath() / filename;
		
		ParticleDescription desc = createParticleDescription(fullpath.string());
		setDescription(desc);
	}

	void Particle::setDescription(const ParticleDescription& desc)
	{
		setTexture(desc.texture);
		_localEmitPos = desc.emitPos;
		_subParticles.clear();

		if (_emitter)
		{
			_emitter->setDescription(desc);
			_emitter->setDeathCallback([this](const Vec2& pos) {
				emitSubParticle(pos);
			});
		}

		ParticleMemory::reserveParticleMemory(desc.particleCount);

		_vertexData.resize(desc.particleCount * 4);
		_indices.resize(desc.particleCount * 6);

		Vec2 uvlt = {0.0f, 0.0f};
		Vec2 uvrb = {1.0f, 1.0f};
		if (_texTile && _texTile->texture())
		{
			Rect src = _texTile->rect();
			Vec2 texSize = _texTile->texture()->size();
			if (texSize.x > 0.0f && texSize.y > 0.0f)
			{
				uvlt = {src.x / texSize.x, src.y / texSize.y};
				uvrb = {(src.x + src.w) / texSize.x, (src.y + src.h) / texSize.y};
			}
		}

		for (int i = 0; i < desc.particleCount; i++)
		{
			_vertexData[i * 4 + 0].tex_coord = { uvlt.x, uvlt.y };
			_vertexData[i * 4 + 1].tex_coord = { uvlt.x, uvrb.y };
			_vertexData[i * 4 + 2].tex_coord = { uvrb.x, uvrb.y };
			_vertexData[i * 4 + 3].tex_coord = { uvrb.x, uvlt.y };

			_indices[i * 6 + 0] = i * 4 + 0;
			_indices[i * 6 + 1] = i * 4 + 2;
			_indices[i * 6 + 2] = i * 4 + 1;
			_indices[i * 6 + 3] = i * 4 + 0;
			_indices[i * 6 + 4] = i * 4 + 3;
			_indices[i * 6 + 5] = i * 4 + 2;
		}

		_descript = desc;
	}

	void Particle::update(float dt)
	{
		if (_emitter)
		{
			_emitter->update(dt);
		}

		for (auto it = _subParticles.begin(); it != _subParticles.end(); )
		{
			auto& child = *it;
			if (!child)
			{
				it = _subParticles.erase(it);
				continue;
			}

			child->update(dt);
			if (!child->isPlaying())
			{
				it = _subParticles.erase(it);
			}
			else
			{
				++it;
			}
		}
	}

	void Particle::draw(engine::Camera* camera, bool screenSpace)
	{
		if (!_emitter || !_texTile || !_texTile->texture())
		{
			return;
		}

		auto particleList = _emitter->getParticleList();
		drawParticleList(particleList, camera, screenSpace);

		for (auto& child : _subParticles)
		{
			if (child)
			{
				child->draw(camera, screenSpace);
			}
		}
	}

	void Particle::drawParticleList(const std::vector<ParticleCfg*>* particleList, engine::Camera* camera, bool screenSpace)
	{
		if (!particleList || particleList->empty() || !_texTile || !_texTile->texture())
		{
			return;
		}

		bool useScreenSpace = screenSpace;
		if (_descript.coordinateSpace == CoordinateSpace::World)
		{
			useScreenSpace = false;
		}
		else if (_descript.coordinateSpace == CoordinateSpace::Screen)
		{
			useScreenSpace = true;
		}

		Texture* texture = _texTile->texture();
		Vec2 center = _texTile->center();
		float s = 0, c = 0, x = 0, y = 0;
		int i = 0;

		for ( auto& particle : *particleList)
		{
			assert(particle);
			fail_continue(particle);

			Vec2 p1;
			Vec2 p2;
			Vec2 p3;
			Vec2 p4;
			Color color = particle->color;
			Color tailColor = color;

			if (_descript.trailMode == TrailMode::Stretch && _descript.trailLength > 0.0f && lengthSquared(particle->velocity) > 0.000001f)
			{
				Vec2 dir = safeNormal(particle->velocity);
				Vec2 normal = {-dir.y, dir.x};
				float width = _descript.trailWidth > 0.0f ? _descript.trailWidth : particle->size;
				Vec2 head = particle->pos;
				Vec2 tail = particle->pos - dir * _descript.trailLength;
				Vec2 halfWidth = normal * (width * 0.5f);

				p1 = tail - halfWidth;
				p2 = head - halfWidth;
				p3 = head + halfWidth;
				p4 = tail + halfWidth;
				tailColor.a = static_cast<uint8_t>(std::clamp(tailColor.a * _descript.trailAlphaScale, 0.0f, 255.0f));
			}
			else
			{
				float left = -particle->size * center.x;
				float right = particle->size * (1.0f - center.x);
				float top = -particle->size * center.y;
				float bottom = particle->size * (1.0f - center.y);
				c = cosf(particle->rotation);
				s = sinf(particle->rotation);

				x = particle->pos.x;
				y = particle->pos.y;

				p1 = {x + left * c + top * s, y - left * s + top * c};
				p2 = {x + left * c + bottom * s, y - left * s + bottom * c};
				p3 = {x + right * c + bottom * s, y - right * s + bottom * c};
				p4 = {x + right * c + top * s, y - right * s + top * c};
			}

			if(camera)
			{
				if (useScreenSpace)
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

			color.a = static_cast<uint8_t>(std::clamp(color.a * _alphaScale, 0.0f, 255.0f));
			tailColor.a = static_cast<uint8_t>(std::clamp(tailColor.a * _alphaScale, 0.0f, 255.0f));
			_vertexData[i * 4 + 0].color = tailColor;
			_vertexData[i * 4 + 1].color = color;
			_vertexData[i * 4 + 2].color = color;
			_vertexData[i * 4 + 3].color = tailColor;

			i++;
		}

		int vex_count = particleList->size() * 4;
		int idx_count = particleList->size() * 6;

		auto& renderer = ParticleManager::inst().getApplication()->renderer();
		SDL_BlendMode oldBlendMode = SDL_BLENDMODE_BLEND;
		if (texture->texture())
		{
			SDL_GetTextureBlendMode(texture->texture(), &oldBlendMode);
			SDL_SetTextureBlendMode(texture->texture(), (SDL_BlendMode)_descript.blendMode);
		}
		renderer.drawGeometry(texture, _vertexData.data(), vex_count, _indices.data(), idx_count);
		if (texture->texture())
		{
			SDL_SetTextureBlendMode(texture->texture(), oldBlendMode);
		}
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
			LogWarn("Particle::createParticleDescription: load xml ({}) failed, err = {}.", filename, (int)error);
			return desc;
		}

		XMLElement* root = doc.RootElement();
		fail_return_result(root, desc);

		XMLElement* secEle = root->FirstChildElement();
		while (secEle)
		{
			XMLElement* propEle = secEle->FirstChildElement("prop");
			while (propEle)
			{
				const char* propName = propEle->Attribute("name");
				const char* propValue = propEle->Attribute("value");
				if (propName && propValue)
				{
					props[propName] = propValue;
				}

				propEle = propEle->NextSiblingElement("prop");
			}

			secEle = secEle->NextSiblingElement();
		}

		// desc
		desc.texture = getString(props, "texture_file");
		desc.coordinateSpace = (CoordinateSpace)getInt(props, "coordinate_space", (int)CoordinateSpace::Inherit);
		desc.blendMode = (BlendMode)getInt(props, "blend_mode", (int)BlendMode::Blend);

		// emmiter
		desc.emitPos = getVec2(props, "start_pos");
		desc.emitPosVar = getVec2(props, "start_pos_variance");

		desc.emitterShape = (EmitterShape)getInt(props, "emitter_shape", (int)EmitterShape::Rect);
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

		desc.emitterType = (EmitterType)getInt(props, "emit_type", (int)EmitterType::Gravity);
		desc.duration = getFloat(props, "duration", -1.0f);

		desc.particleCount = getInt(props, "max_particles");
		desc.emitRate = getFloat(props, "emission_rate");

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
		desc.colorCurve = (ValueCurve)getInt(props, "color_curve", (int)ValueCurve::Linear);
		desc.sizeCurve = (ValueCurve)getInt(props, "size_curve", (int)ValueCurve::Linear);
		desc.spinCurve = (ValueCurve)getInt(props, "spin_curve", (int)ValueCurve::Linear);
		desc.trailMode = (TrailMode)getInt(props, "trail_mode", (int)TrailMode::None);
		desc.trailLength = getFloat(props, "trail_length");
		desc.trailWidth = getFloat(props, "trail_width");
		desc.trailAlphaScale = getFloat(props, "trail_alpha_scale", 0.35f);
		desc.subEmitter = getString(props, "sub_emitter");
		desc.subEmitterCount = getInt(props, "sub_emitter_count", 1);

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
		desc.windMode.force = getVec2(props, "wind");
		desc.windMode.forceVar = getVec2(props, "wind_variance");
		desc.windMode.wave = getVec2(props, "wind_wave");
		desc.windMode.waveSpeed = getFloat(props, "wind_wave_speed");

		correctDescription(desc);

		return desc;
	}

	// 修正一些数据
	void Particle::correctDescription(ParticleDescription& desc)
	{
		int emitterShape = std::clamp((int)desc.emitterShape, (int)EmitterShape::Point, (int)EmitterShape::Circle);
		desc.emitterShape = (EmitterShape)emitterShape;

		int coordinateSpace = std::clamp((int)desc.coordinateSpace, (int)CoordinateSpace::Inherit, (int)CoordinateSpace::Screen);
		desc.coordinateSpace = (CoordinateSpace)coordinateSpace;

		switch (desc.blendMode)
		{
		case BlendMode::None:
		case BlendMode::Blend:
		case BlendMode::BlendPremultiplied:
		case BlendMode::Add:
		case BlendMode::AddPremultiplied:
		case BlendMode::Mod:
		case BlendMode::Mul:
			break;
		default:
			desc.blendMode = BlendMode::Blend;
			break;
		}

		int emitterType = std::clamp((int)desc.emitterType, (int)EmitterType::Gravity, (int)EmitterType::Radius);
		desc.emitterType = (EmitterType)emitterType;

		int motionMode = std::clamp((int)desc.motionMode, (int)MotionMode::Free, (int)MotionMode::Relative);
		desc.motionMode = (MotionMode)motionMode;

		int colorCurve = std::clamp((int)desc.colorCurve, (int)ValueCurve::Linear, (int)ValueCurve::Pulse);
		desc.colorCurve = (ValueCurve)colorCurve;

		int sizeCurve = std::clamp((int)desc.sizeCurve, (int)ValueCurve::Linear, (int)ValueCurve::Pulse);
		desc.sizeCurve = (ValueCurve)sizeCurve;

		int spinCurve = std::clamp((int)desc.spinCurve, (int)ValueCurve::Linear, (int)ValueCurve::Pulse);
		desc.spinCurve = (ValueCurve)spinCurve;

		int trailMode = std::clamp((int)desc.trailMode, (int)TrailMode::None, (int)TrailMode::Stretch);
		desc.trailMode = (TrailMode)trailMode;

		desc.particleCount = std::max(0, desc.particleCount);
		desc.life = desc.life <= 0.0f ? 0.1f : desc.life;
		desc.lifeVar = std::max(0.0f, desc.lifeVar);
		desc.emitShapeRadius = std::max(0.0f, desc.emitShapeRadius);
		desc.emitShapeSize.x = std::max(0.0f, desc.emitShapeSize.x);
		desc.emitShapeSize.y = std::max(0.0f, desc.emitShapeSize.y);
		desc.trailLength = std::max(0.0f, desc.trailLength);
		desc.trailWidth = std::max(0.0f, desc.trailWidth);
		desc.trailAlphaScale = std::clamp(desc.trailAlphaScale, 0.0f, 1.0f);
		desc.subEmitterCount = std::max(0, desc.subEmitterCount);

		if (desc.emitRate <= 0.0f)
		{
			desc.emitRate = desc.particleCount / desc.life;
		}
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
		for (auto& child : _subParticles)
		{
			if (child)
			{
				child->stop();
			}
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
		if (_emitter && _emitter->isActive())
		{
			return true;
		}

		for (auto& child : _subParticles)
		{
			if (child && child->isPlaying())
			{
				return true;
			}
		}

		return false;
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
		return _emitter ? _emitter->getParticleEffect() : nullptr;
	}

	ParticleEmitter* Particle::getEmitter()
	{
		return _emitter.get();
	}

	void Particle::emitSubParticle(const Vec2& pos)
	{
		if (_descript.subEmitter.empty() || _descript.subEmitterCount <= 0)
		{
			return;
		}

		for (int i = 0; i < _descript.subEmitterCount; ++i)
		{
			auto child = ParticleManager::inst().createParticle(_descript.subEmitter);
			if (!child)
			{
				LogWarn("Particle::emitSubParticle: create child particle ({}) failed.", _descript.subEmitter);
				return;
			}

			child->setPos(pos);
			child->setAlphaScale(_alphaScale);
			child->start();
			_subParticles.push_back(child);
		}
	}

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
		props["coordinate_space"] = std::to_string((int)_descript.coordinateSpace);
		props["blend_mode"] = std::to_string((int)_descript.blendMode);
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
		props["emission_rate"] = std::to_string(_descript.emitRate);

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
		props["color_curve"] = std::to_string((int)_descript.colorCurve);
		props["size_curve"] = std::to_string((int)_descript.sizeCurve);
		props["spin_curve"] = std::to_string((int)_descript.spinCurve);
		props["trail_mode"] = std::to_string((int)_descript.trailMode);
		props["trail_length"] = std::to_string(_descript.trailLength);
		props["trail_width"] = std::to_string(_descript.trailWidth);
		props["trail_alpha_scale"] = std::to_string(_descript.trailAlphaScale);
		props["sub_emitter"] = _descript.subEmitter;
		props["sub_emitter_count"] = std::to_string(_descript.subEmitterCount);

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
		props["wind"] = vec2ToString(_descript.windMode.force);
		props["wind_variance"] = vec2ToString(_descript.windMode.forceVar);
		props["wind_wave"] = vec2ToString(_descript.windMode.wave);
		props["wind_wave_speed"] = std::to_string(_descript.windMode.waveSpeed);


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
