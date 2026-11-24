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
		virtual ~ParticleEffect() = default;

		void setMotionMode(MotionMode mode);
		void setDescription(const ParticleDescription& desc);

		// 初始化单个粒子。
		virtual void initParticle(ParticleEmitter* pe, ParticleCfg* particle);

		// 更新当前 effect 管理的粒子。
		virtual void update(ParticleEmitter* pe, float dt) = 0;

	public:
		// 粒子生命周期。
		VirtualMemberFunction(float, Life, _life);
		VirtualMemberFunction(float, LifeVar, _lifeVar);

		// 粒子起始颜色。
		VirtualMemberFunction(Color, BeginColor, _beginColor);
		VirtualMemberFunction(Color, BeginColorVar, _beginColorVar);

		// 粒子结束颜色。
		VirtualMemberFunction(Color, EndColor, _endColor);
		VirtualMemberFunction(Color, EndColorVar, _endColorVar);

		// 粒子起始大小。
		VirtualMemberFunction(float, BeginSize, _beginSize);
		VirtualMemberFunction(float, BeginSizeVar, _beginSizeVar);

		// 粒子结束大小。
		VirtualMemberFunction(float, EndSize, _endSize);
		VirtualMemberFunction(float, EndSizeVar, _endSizeVar);

		// 粒子起始旋转角度。
		VirtualMemberFunction(float, BeginSpin, _beginSpin);
		VirtualMemberFunction(float, BeginSpinVar, _beginSpinVar);

		// 粒子结束旋转角度。
		VirtualMemberFunction(float, EndSpin, _endSpin);
		VirtualMemberFunction(float, EndSpinVar, _endSpinVar);

	protected:
		MotionMode _motionMode;

		GravityMode _gravityMode;
		RadiusMode _radiusMode;
	};

	// 重力模式粒子效果。
	class GravityParticleEffect : public ParticleEffect
	{
	public:
		void initParticle(ParticleEmitter* pe, ParticleCfg* particle) override;
		void update(ParticleEmitter* pe, float dt) override;
	};

	// 半径模式粒子效果。
	class RadialParticleEffect : public ParticleEffect
	{
	public:
		void initParticle(ParticleEmitter* pe, ParticleCfg* particle) override;
		void update(ParticleEmitter* pe, float dt) override;
	};
}
