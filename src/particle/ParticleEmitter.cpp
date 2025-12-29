#include "ParticleEmitter.h"
#include "ParticleEffect.h"
#include "ParticleMemory.h"

namespace particle
{
	ParticleEmitter::ParticleEmitter()
		: pParticleEffect(nullptr)
		, bCanEmit(true)
		, fElapsed(0)
		, fEmitCounter(0)
	{
	}

	ParticleEmitter::~ParticleEmitter()
	{
		if ( pParticleEffect ) {
			delete pParticleEffect;
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
		if ( pParticleEffect ) {
			delete pParticleEffect;
		}
		pParticleEffect = effect;
	}

	void ParticleEmitter::update(float dt)
	{
		if ( bCanEmit == false ) return;

		this->emitParticles(dt);
		if (pParticleEffect) {
			pParticleEffect->update(this, dt);
		}
	}

	void ParticleEmitter::emitParticles(float dt)
	{
		assert(emitRate!=0);

		/* 发射一个粒子所用时间 */
		float emit_particle_time = 1 / emitRate;

		/* 累计发射时间 */
		if ( vParticleList.size() < particleCount ) {
			fEmitCounter += dt;
		}

		/* 在时间 emit_counter 发射 emit_counter / rate 个粒子 */
		while ( vParticleList.size() < particleCount && fEmitCounter > 0 ) {
			this->addParticle();
			fEmitCounter -= emit_particle_time;
		}

		fElapsed += dt;
		if ( duration != -1 && duration < fElapsed ) {
			fElapsed = 0;
			this->stopEmitting();
		}
	}

	void ParticleEmitter::addParticle()
	{
		if ( vParticleList.size() == particleCount ) return;

		ParticleCfg* particle = ParticleMemory::allocParticle();
		fail_return(particle);

		/* 存储粒子并初始化粒子 */
		vParticleList.push_back(particle);
		pParticleEffect->initParticle(this, particle);
	}


	bool ParticleEmitter::isEmitting()
	{
		return bCanEmit;
	}
	
	void ParticleEmitter::startEmitting()
	{
		bCanEmit = true;
	}

	void ParticleEmitter::stopEmitting()
	{
		bCanEmit = false;

		/* 释放所有未发射的粒子 */
		for ( auto& ele : vParticleList ) {
			ParticleMemory::freeParticle(ele);
		}
		vParticleList.clear();
	}
}