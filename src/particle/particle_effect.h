#pragma once
#include "particle_cfg.h"

namespace particle
{
	class ParticleEmitter;
	class Particle;

	class ParticleEffect
	{
	public:
		ParticleEffect();
		virtual ~ParticleEffect() {}

		void setDecription(const ParticleDescription& desc);

		/* 初始化粒子 */
		virtual void initParticle(ParticleEmitter* pe, ParticleCfg* particle);

		/* 更新粒子 */
		virtual void update(ParticleEmitter* pe, float dt) = 0;

		void setMotionMode(MotionMode mode) { motionMode = mode; }

	public:
		/* 粒子生命周期 */
		VirtualMemberFunction(float, Life, life);
		VirtualMemberFunction(float, LifeVar, lifeVar);

		/* 粒子起始颜色  */
		VirtualMemberFunction(Color4f, BeginColor, beginColor);
		VirtualMemberFunction(Color4f, BeginColorVar, beginColorVar);

		/* 粒子结束颜色 */
		VirtualMemberFunction(Color4f, EndColor, endColor);
		VirtualMemberFunction(Color4f, EndColorVar, endColorVar);

		/* 粒子起始大小 */
		VirtualMemberFunction(float, BeginSize, beginSize);
		VirtualMemberFunction(float, BeginSizeVar, beginSizeVar);

		/* 粒子结束大小 */
		VirtualMemberFunction(float, EndSize, endSize);
		VirtualMemberFunction(float, EndSizeVar, endSizeVar);

		/* 粒子起始旋转角度 */
		VirtualMemberFunction(float, BeginSpin, beginSpin);
		VirtualMemberFunction(float, BeginSpinVar, beginSpinVar);

		/* 粒子结束旋转角度 */
		VirtualMemberFunction(float, EndSpin, endSpin);
		VirtualMemberFunction(float, EndSpinVar, endSpinVar);

	public:
		MotionMode motionMode;

		GravityMode gravityMode;
		RadiusMode radiusMode;
	};

	//---------------------------------------------------------------------
	// GravityParticleEffect
	//---------------------------------------------------------------------
	class GravityParticleEffect : public ParticleEffect
	{
	public:
		void initParticle(ParticleEmitter* pe, ParticleCfg* particle) override;
		void update(ParticleEmitter* pe, float dt) override;
	};

	//---------------------------------------------------------------------
	// RadialParticleEffect
	//---------------------------------------------------------------------
	class RadialParticleEffect : public ParticleEffect
	{
	public:
		void initParticle(ParticleEmitter* pe, ParticleCfg* particle) override;
		void update(ParticleEmitter* pe, float dt) override;
	};
}