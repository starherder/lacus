#include "particle_emitter.h"
#include "particle_effect.h"
#include "particle_memory.h"

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
		stopEmitting();

		if ( _particleEffect ) {
			delete _particleEffect;
		}
	}

	void ParticleEmitter::setDecription(const ParticleDescription& desc)
	{
		/* 发射器属性 */
		emitPos = desc.vEmitPos;
		emitPosVar = desc.vEmitPosVar;

		emitAngle = desc.fEmitAngle;
		emitAngleVar = desc.fEmitAngleVar;

		emitSpeed = desc.fEmitSpeed;
		emitSpeedVar = desc.fEmitSpeedVar;

		emitShape = desc.emitterShape;
		emitShapeSize = desc.vEmitShapeSize;
		emitShapeRadius = desc.fEmitShapeRadius;
		emitShapeRotation = desc.fEmitShapeRotation;
		emitShapeEdgeOnly = desc.bEmitShapeEdgeOnly;

		emitRate = desc.fEmitRate;
		duration = desc.fDuration;
		particleCount = desc.nParticleCount;

		/* 创建粒子 effect */
		ParticleEffect* effect = nullptr;
		if ( desc.emitterType == EmitterType::EMITTER_TYPE_GRAVITY ) {
			effect = new GravityParticleEffect();
		}
		else {
			effect = new RadialParticleEffect();
		}

		assert(effect);
		fail_return(effect);

		effect->setDecription(desc);
		this->setParticleEffect(effect);
	}

	void ParticleEmitter::setParticleEffect(ParticleEffect* effect)
	{
		if ( _particleEffect ) {
			delete _particleEffect;
		}
		_particleEffect = effect;
	}

	void ParticleEmitter::update(float dt)
	{
		if ( _canEmit == false ) return;

		this->emitParticles(dt);
		if (_particleEffect) {
			_particleEffect->update(this, dt);
		}
	}

	void ParticleEmitter::emitParticles(float dt)
	{
		emitRate = emitRate == 0 ? 1 : emitRate;

		/* 发射一个粒子所用时间 */
		float emit_particle_time = 1 / emitRate;

		/* 累计发射时间 */
		if ( _particleList.size() < particleCount ) {
			_emitCounter += dt;
		}

		/* 在时间 emit_counter 发射 emit_counter / rate 个粒子 */
		while ( _particleList.size() < particleCount && _emitCounter > 0 ) {
			this->addParticle();
			_emitCounter -= emit_particle_time;
		}

		_elapsed += dt;
		if ( duration != -1 && duration < _elapsed ) {
			_elapsed = 0;
			this->stopEmitting();
		}
	}

	void ParticleEmitter::addParticle()
	{
		if ( _particleList.size() == particleCount ) return;

		ParticleCfg* particle = ParticleMemory::allocParticle();
		fail_return(particle);

		/* 存储粒子并初始化粒子 */
		_particleList.push_back(particle);
		_particleEffect->initParticle(this, particle);
	}

	Vec2 ParticleEmitter::randomEmitPos() const
	{
		switch (emitShape)
		{
		case EmitterShape::EMITTER_SHAPE_POINT:
		{
			return emitPos;
		}break;
		case EmitterShape::EMITTER_SHAPE_LINE:
		{
			Vec2 offset{ emitShapeSize.x * utility::rand_minus1_1() * 0.5f, 0.0f };
			return emitPos + rotateVec2(offset, emitShapeRotation);
		}break;
		case EmitterShape::EMITTER_SHAPE_CIRCLE:
		{
			float angle = PI_2 * utility::rand_0_1();
			float radius = std::max(0.0f, emitShapeRadius);
			float dist = emitShapeEdgeOnly ? radius : radius * sqrtf(utility::rand_0_1());
			return emitPos + Vec2{ cosf(angle), sinf(angle) } * dist;
		}break;
		case EmitterShape::EMITTER_SHAPE_RECT:
		default:
		{
			Vec2 offset{
				emitShapeSize.x * utility::rand_minus1_1() * 0.5f,
				emitShapeSize.y * utility::rand_minus1_1() * 0.5f
			};

			if (emitShapeEdgeOnly)
			{
				if (utility::rand_0_1() < 0.5f) {
					offset.x = emitShapeSize.x * 0.5f * (utility::rand_0_1() < 0.5f ? -1.0f : 1.0f);
				}
				else {
					offset.y = emitShapeSize.y * 0.5f * (utility::rand_0_1() < 0.5f ? -1.0f : 1.0f);
				}
			}

			return emitPos + rotateVec2(offset, emitShapeRotation);
		}break;
		}

		return emitPos;
	}


	bool ParticleEmitter::isEmitting()
	{
		return _canEmit;
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

		/* 释放所有未发射的粒子 */
		for ( auto& ele : _particleList ) {
			ParticleMemory::freeParticle(ele);
		}
		_particleList.clear();

		_elapsed = 0;
		_emitCounter = 0;
	}
}
