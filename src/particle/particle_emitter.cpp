#include "particle_emitter.h"
#include "particle_effect.h"
#include "particle_memory.h"

namespace particle
{
	ParticleEmitter::ParticleEmitter()
		: _particleEffect(nullptr)
		, _canEmit(true)
		, _elapsed(0)
		, _emitCounter(0)
	{
	}

	ParticleEmitter::~ParticleEmitter()
	{
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


	bool ParticleEmitter::isEmitting()
	{
		return _canEmit;
	}
	
	void ParticleEmitter::startEmitting()
	{
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
	}
}