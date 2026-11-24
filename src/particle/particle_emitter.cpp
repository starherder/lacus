#include "particle_emitter.h"
#include "particle_effect.h"
#include "particle_memory.h"

#include "engine/i_painter.h"

namespace particle
{
	static Vec2 rotateVec2(const Vec2& vec, float degree)
	{
		float radian = DegreeToRadian(degree);
		float c = cosf(radian);
		float s = sinf(radian);
		return Vec2{ vec.x * c - vec.y * s, vec.x * s + vec.y * c };
	}

	ParticleEmitter::ParticleEmitter()
		: _particleEffect(nullptr)
		, _canEmit(true)
		, _elapsed(0)
		, _emitCounter(0)
		{
	}

	ParticleEmitter::~ParticleEmitter()
	{
		clearParticles();
	}

	void ParticleEmitter::setDescription(const ParticleDescription& desc)
	{
		// 应用发射器属性。
		_emitPos = desc.emitPos;
		_emitPosVar = desc.emitPosVar;

		_emitAngle = desc.emitAngle;
		_emitAngleVar = desc.emitAngleVar;

		_emitSpeed = desc.emitSpeed;
		_emitSpeedVar = desc.emitSpeedVar;

		_emitShape = desc.emitterShape;
		_emitShapeSize = desc.emitShapeSize;
		_emitShapeRadius = desc.emitShapeRadius;
		_emitShapeRotation = desc.emitShapeRotation;
		_emitShapeEdgeOnly = desc.emitShapeEdgeOnly;

		_emitRate = desc.emitRate;
		_duration = desc.duration;
		_particleCount = desc.particleCount;

		// 根据发射器类型创建 effect。
		std::unique_ptr<ParticleEffect> effect;
		if ( desc.emitterType == EmitterType::Gravity )
		{
			effect = std::make_unique<GravityParticleEffect>();
		}
		else
		{
			effect = std::make_unique<RadialParticleEffect>();
		}

		assert(effect);
		fail_return(effect);

		effect->setDescription(desc);
		this->setParticleEffect(std::move(effect));
		clearParticles();
	}

	void ParticleEmitter::setParticleEffect(std::unique_ptr<ParticleEffect> effect)
	{
		_particleEffect = std::move(effect);
	}

	void ParticleEmitter::setDeathCallback(std::function<void(const Vec2&)> callback)
	{
		_deathCallback = std::move(callback);
	}

	void ParticleEmitter::emitParticleDeath(const Vec2& pos)
	{
		if (_deathCallback)
		{
			_deathCallback(pos);
		}
	}

	void ParticleEmitter::update(float dt)
	{
		if ( _canEmit )
		{
			this->emitParticles(dt);
		}
		if (_particleEffect)
		{
			_particleEffect->update(this, dt);
		}
	}

	void ParticleEmitter::drawDebug(engine::IPainter& painter, const Color& color, float thickness) const
	{
		const float pointSize = 8.0f;
		painter.drawLine(color, _emitPos + Vec2{-pointSize, 0.0f}, _emitPos + Vec2{pointSize, 0.0f}, thickness);
		painter.drawLine(color, _emitPos + Vec2{0.0f, -pointSize}, _emitPos + Vec2{0.0f, pointSize}, thickness);

		switch (_emitShape)
		{
		case EmitterShape::Point:
			break;
		case EmitterShape::Line:
		{
			Vec2 half = rotateVec2({_emitShapeSize.x * 0.5f, 0.0f}, _emitShapeRotation);
			painter.drawLine(color, _emitPos - half, _emitPos + half, thickness);
		}
		break;
		case EmitterShape::Circle:
		{
			painter.drawCircle(color, _emitPos, std::max(0.0f, _emitShapeRadius), 48, thickness);
		}
		break;
		case EmitterShape::Rect:
		default:
		{
			Vec2 half = _emitShapeSize * 0.5f;
			Vec2 points[] = {
				_emitPos + rotateVec2({-half.x, -half.y}, _emitShapeRotation),
				_emitPos + rotateVec2({ half.x, -half.y}, _emitShapeRotation),
				_emitPos + rotateVec2({ half.x,  half.y}, _emitShapeRotation),
				_emitPos + rotateVec2({-half.x,  half.y}, _emitShapeRotation)
			};
			painter.drawLines(color, points, 4, true, thickness);
		}
		break;
		}
	}

	void ParticleEmitter::emitParticles(float dt)
	{
		float emitRate = _emitRate <= 0.0f ? 1.0f : _emitRate;

		// 单个粒子的发射间隔。
		float emit_particle_time = 1.0f / emitRate;

		// 累计待发射时间。
		if ( _particleList.size() < _particleCount )
		{
			_emitCounter += dt;
		}

		// 按累计时间补发粒子。
		while ( _particleList.size() < _particleCount && _emitCounter >= emit_particle_time )
		{
			this->addParticle();
			_emitCounter -= emit_particle_time;
		}

		_elapsed += dt;
		if ( _duration != -1 && _duration < _elapsed )
		{
			_elapsed = 0;
			_canEmit = false;
		}
	}

	void ParticleEmitter::addParticle()
	{
		if ( _particleList.size() == _particleCount ) return;
		fail_return(_particleEffect);

		ParticleCfg* particle = ParticleMemory::allocParticle();
		fail_return(particle);

		// 存储粒子并初始化。
		_particleList.push_back(particle);
		_particleEffect->initParticle(this, particle);
	}

	Vec2 ParticleEmitter::randomEmitPos() const
	{
		switch (_emitShape)
		{
		case EmitterShape::Point:
		{
			return _emitPos;
		}
		break;
		case EmitterShape::Line:
		{
			Vec2 offset{ _emitShapeSize.x * utility::rand_minus1_1() * 0.5f, 0.0f };
			return _emitPos + rotateVec2(offset, _emitShapeRotation);
		}
		break;
		case EmitterShape::Circle:
		{
			float angle = PI_2 * utility::rand_0_1();
			float radius = std::max(0.0f, _emitShapeRadius);
			float dist = _emitShapeEdgeOnly ? radius : radius * sqrtf(utility::rand_0_1());
			return _emitPos + Vec2{ cosf(angle), sinf(angle) } * dist;
		}
		break;
		case EmitterShape::Rect:
		default:
		{
			Vec2 offset{
				_emitShapeSize.x * utility::rand_minus1_1() * 0.5f,
				_emitShapeSize.y * utility::rand_minus1_1() * 0.5f
			};

			if (_emitShapeEdgeOnly)
			{
				if (utility::rand_0_1() < 0.5f)
				{
					offset.x = _emitShapeSize.x * 0.5f * (utility::rand_0_1() < 0.5f ? -1.0f : 1.0f);
				}
				else
				{
					offset.y = _emitShapeSize.y * 0.5f * (utility::rand_0_1() < 0.5f ? -1.0f : 1.0f);
				}
			}

			return _emitPos + rotateVec2(offset, _emitShapeRotation);
		}
		break;
		}

		return _emitPos;
	}


	bool ParticleEmitter::isEmitting()

	{
		return _canEmit;
	}

	bool ParticleEmitter::isActive() const
	{
		return _canEmit || !_particleList.empty();
	}
	
	void ParticleEmitter::startEmitting()
	{
		_elapsed = 0;
		_emitCounter = 0;
		_canEmit = true;
	}

	void ParticleEmitter::stopEmitting()
	{
		_canEmit = false;
		_elapsed = 0;
		_emitCounter = 0;
	}

	void ParticleEmitter::clearParticles()
	{
		// 释放仍在发射器中的粒子。
		for ( auto& ele : _particleList )
		{
			ParticleMemory::freeParticle(ele);
		}
		_particleList.clear();

		_elapsed = 0;
		_emitCounter = 0;
	}

	std::vector<ParticleCfg*>* ParticleEmitter::getParticleList()
	{
		return &_particleList;
	}

	ParticleEffect* ParticleEmitter::getParticleEffect()
	{
		return _particleEffect.get();
	}
}
