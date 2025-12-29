#pragma once
#include "ParticleCfg.h"

#include <list>

namespace particle
{
	class ParticleEffect;

	class ParticleEmitter
	{
	public:
		ParticleEmitter();
		~ParticleEmitter();

		void setDecription(const ParticleDescription& desc);
		void setParticleEffect(ParticleEffect* effect);

		void emitParticles(float dt);
		void addParticle();

		void update(float dt);

		void startEmitting();
		void stopEmitting();

		bool isEmitting();

		std::list<ParticleCfg*>* getParticleList() { return &vParticleList; }
		ParticleEffect* getParticleEffect() { return pParticleEffect; }

		//===================== 发射器属性 =====================

		/* 发射器位置 */
		MemberFunction(Vec2, EmitPos, emitPos);
		MemberFunction(Vec2, EmitPosVar, emitPosVar);

		/* 发射器发射粒子角度 */
		MemberFunction(float, EmitAngle, emitAngle);
		MemberFunction(float, EmitAngleVar, emitAngleVar);

		/* 发射器发射粒子给粒子的初速度大小 */
		MemberFunction(float, EmitSpeed, emitSpeed);
		MemberFunction(float, EmitSpeedVar, emitSpeedVar);

		/* 粒子数量 */
		MemberFunction(int, ParticleCount, particleCount);

		/* 发射速率  */
		MemberFunction(float, EmitRate, emitRate);

		/* 发射持续时间，-1 表示永远发射 */
		MemberFunction(float, Duration, duration);

	private:
		ParticleEffect* pParticleEffect;
		std::list<ParticleCfg*> vParticleList;

		bool bCanEmit;
		float fElapsed;
		float fEmitCounter;
	};
}