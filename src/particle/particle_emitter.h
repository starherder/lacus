#pragma once
#include "particle_cfg.h"

#include <functional>
#include <memory>
#include <vector>

namespace engine
{
	class IPainter;
}

namespace particle
{
	class ParticleEffect;

	class ParticleEmitter
	{
	public:
		ParticleEmitter();
		~ParticleEmitter();

		void setDescription(const ParticleDescription& desc);
		void setParticleEffect(std::unique_ptr<ParticleEffect> effect);
		void setDeathCallback(std::function<void(const Vec2&)> callback);
		void emitParticleDeath(const Vec2& pos);

		void emitParticles(float dt);
		void addParticle();
		Vec2 randomEmitPos() const;

		void update(float dt);
		void drawDebug(engine::IPainter& painter, const Color& color, float thickness = 1.0f) const;

		void startEmitting();
		void stopEmitting();
		void clearParticles();

		bool isEmitting();
		bool isActive() const;

		std::vector<ParticleCfg*>* getParticleList();
		ParticleEffect* getParticleEffect();

		// 发射器位置。
		MemberFunction(Vec2, EmitPos, _emitPos);
		MemberFunction(Vec2, EmitPosVar, _emitPosVar);

		// 发射角度。
		MemberFunction(float, EmitAngle, _emitAngle);
		MemberFunction(float, EmitAngleVar, _emitAngleVar);

		// 发射初速度。
		MemberFunction(float, EmitSpeed, _emitSpeed);
		MemberFunction(float, EmitSpeedVar, _emitSpeedVar);

		// 发射器形状。
		MemberFunction(EmitterShape, EmitShape, _emitShape);
		MemberFunction(Vec2, EmitShapeSize, _emitShapeSize);
		MemberFunction(float, EmitShapeRadius, _emitShapeRadius);
		MemberFunction(float, EmitShapeRotation, _emitShapeRotation);
		MemberFunction(bool, EmitShapeEdgeOnly, _emitShapeEdgeOnly);

		// 粒子数量。
		MemberFunction(int, ParticleCount, _particleCount);

		// 每秒发射速率。
		MemberFunction(float, EmitRate, _emitRate);

		// 发射持续时间，-1 表示永远发射。
		MemberFunction(float, Duration, _duration);

	private:
		std::unique_ptr<ParticleEffect> _particleEffect;
		std::vector<ParticleCfg*> _particleList;
		std::function<void(const Vec2&)> _deathCallback;

		bool _canEmit;
		float _elapsed;
		float _emitCounter;
	};
}
