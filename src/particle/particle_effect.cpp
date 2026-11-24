#include "particle_effect.h"
#include "particle_emitter.h"
#include "particle_manager.h"
#include "particle_memory.h"

namespace particle
{
	ParticleEffect::ParticleEffect()
	{
		_motionMode = MotionMode::Free;
	}

	void ParticleEffect::setDescription(const ParticleDescription& desc)
	{
		_life = desc.life;
		_lifeVar = desc.lifeVar;

		_beginColor = desc.beginColor;
		_beginColorVar = desc.beginColorVar;
		_endColor = desc.endColor;
		_endColorVar = desc.endColorVar;

		_beginSize = desc.beginSize;
		_beginSizeVar = desc.beginSizeVar;
		_endSize = desc.endSize;
		_endSizeVar = desc.endSizeVar;

		_beginSpin = desc.beginSpin;
		_beginSpinVar = desc.beginSpinVar;
		_endSpin = desc.endSpin;
		_endSpinVar = desc.endSpinVar;

		_motionMode = desc.motionMode;

		_gravityMode = desc.gravityMode;
		_radiusMode = desc.radiusMode;
	}

	void ParticleEffect::setMotionMode(MotionMode mode)
	{
		_motionMode = mode;
	}

	void ParticleEffect::initParticle(ParticleEmitter* pe, ParticleCfg* particle)
	{
		assert(pe && particle);
		fail_return(pe && particle);

		// 初始化粒子起始位置。
		particle->pos = pe->randomEmitPos();

		particle->startPos = pe->getEmitPos();
		particle->changePos = particle->pos;

		// 初始化粒子生命周期。
		particle->remainingLife = std::max(0.1f, _life + _lifeVar * utility::rand_minus1_1());

		// 初始化颜色渐变。
		Color begin_color = _beginColor + _beginColorVar * utility::rand_minus1_1();
		Color end_color = _endColor + _endColorVar * utility::rand_minus1_1();

		assert(particle->remainingLife!=0);

		float tmp = 1 / (particle->remainingLife);
		particle->color = begin_color;
		particle->deltaColor = (end_color - begin_color) * tmp;

		// 初始化大小渐变。
		float begin_size = std::max(0.0f, _beginSize + _beginSizeVar * utility::rand_minus1_1());
		float end_size = std::max(0.0f, _endSize + _endSizeVar * utility::rand_minus1_1());

		particle->size = begin_size;
		particle->deltaSize = (end_size - begin_size) / particle->remainingLife;

		// 初始化旋转渐变。
		float begin_spin = DegreeToRadian(std::max(0.0f, _beginSpin + _beginSpinVar * utility::rand_minus1_1()));
		float end_spin = DegreeToRadian(std::max(0.0f, _endSpin + _endSpinVar * utility::rand_minus1_1()));

		particle->rotation = begin_spin;
		particle->deltaRotation = (end_spin - begin_spin) / particle->remainingLife;
	}

	// 初始化重力模式粒子。
	void GravityParticleEffect::initParticle(ParticleEmitter* pe, ParticleCfg* particle)
	{
		assert(pe && particle);
		fail_return(pe && particle);

		ParticleEffect::initParticle(pe, particle);

		// 计算发射初速度。
		float particleSpeed = pe->getEmitSpeed() + pe->getEmitSpeedVar() * utility::rand_minus1_1();

		// 计算发射方向。
		float angle = pe->getEmitAngle() + pe->getEmitAngleVar() * utility::rand_minus1_1();
		Vec2 particleDirection = Vec2(cosf(DegreeToRadian(angle)), sinf(DegreeToRadian(angle)));
		
		// 写入重力模式运行参数。
		particle->gravityMode.initialVelocity = particleDirection * particleSpeed;

		particle->gravityMode.tangentialAccel = _gravityMode.tangentialAccel + _gravityMode.tangentialAccelVar * utility::rand_minus1_1();
		particle->gravityMode.radialAccel = _gravityMode.radialAccel + _gravityMode.radialAccelVar * utility::rand_minus1_1();
	}

	void GravityParticleEffect::update(ParticleEmitter* pe, float dt)
	{
		assert(pe);
		fail_return(pe);

		std::list<ParticleCfg*>* indexList = pe->getParticleList();

		for ( auto it = indexList->begin(); it != indexList->end(); )
		{
			ParticleCfg* p = (*it);
			assert(p);
			fail_continue(p);

			p->remainingLife -= dt;

			if ( p->remainingLife > 0 )
			{
				Vec2 offset{0,0}, radial{0,0}, tangential{0,0};

				// 计算径向加速度。
				if ( p->changePos.x || p->changePos.y )
				{
					offset = p->gravityMode.initialVelocity;
					radial = SafeNormal(offset);
				}
				tangential = radial;
				radial = radial * p->gravityMode.radialAccel;

				// 计算切向加速度。
				float newy = tangential.x;
				tangential.x = -tangential.y;
				tangential.y = newy;
				tangential = tangential * p->gravityMode.tangentialAccel;

				// 计算合力并移动粒子。
				offset = (radial + tangential + _gravityMode.gravity) * dt;

				p->gravityMode.initialVelocity = p->gravityMode.initialVelocity + offset;
				p->changePos = p->changePos + p->gravityMode.initialVelocity * dt;

				// 更新粒子属性。
				p->color = p->color + p->deltaColor * dt;
				p->size = std::max(0.0f, p->size + p->deltaSize * dt);
				p->rotation = p->rotation + p->deltaRotation * dt;

				if ( _motionMode == MotionMode::Relative )
				{
					Vec2 diff = pe->getEmitPos() - p->startPos;
					p->pos = p->changePos + diff;
				}
				else
				{
					p->pos = p->changePos;
				}
				++it;
			}
			else
			{
				// 移除生命周期结束的粒子。
				ParticleMemory::freeParticle(*it);
				it = indexList->erase(it);
			}
		}
	}

	// 初始化半径模式粒子。
	void RadialParticleEffect::initParticle(ParticleEmitter* pe, ParticleCfg* particle)
	{
		assert(pe && particle);
		fail_return(pe && particle);

		ParticleEffect::initParticle(pe, particle);

		assert(particle->remainingLife!=0);

		float begin_radius = _radiusMode.beginRadius + _radiusMode.beginRadiusVar * utility::rand_minus1_1();
		float end_radius = _radiusMode.endRadius + _radiusMode.endRadiusVar * utility::rand_minus1_1();
		
		particle->radiusMode.radius = begin_radius;
		particle->radiusMode.deltaRadius = (end_radius - begin_radius) / particle->remainingLife;

		float degress = pe->getEmitAngle() + pe->getEmitAngleVar() * utility::rand_minus1_1();
		particle->radiusMode.angle = DegreeToRadian(degress);

		degress = _radiusMode.spinPerSecond + _radiusMode.spinPerSecondVar * utility::rand_minus1_1();
		particle->radiusMode.degreesPerSecond = DegreeToRadian(degress);
	}

	void RadialParticleEffect::update(ParticleEmitter* pe, float dt)
	{
		assert(pe);
		fail_return(pe);

		std::list<ParticleCfg*>* indexList = pe->getParticleList();

		for ( auto it = indexList->begin(); it != indexList->end(); )
		{
			ParticleCfg* p = (*it);
			assert(p);
			fail_continue(p);

			p->remainingLife -= dt;

			if ( p->remainingLife > 0 )
			{
				p->radiusMode.angle += p->radiusMode.degreesPerSecond * dt;
				p->radiusMode.radius += p->radiusMode.deltaRadius * dt;

				p->changePos.x = cosf(p->radiusMode.angle) * p->radiusMode.radius;
				p->changePos.y = sinf(p->radiusMode.angle) * p->radiusMode.radius;
				
				if ( _motionMode == MotionMode::Free )
				{
					p->pos = p->changePos + pe->getEmitPos();
				}
				else
				{
					p->pos = p->changePos + p->startPos;
				}

				// 更新粒子属性。
				p->color = p->color + p->deltaColor * dt;
				p->size = std::max(0.0f, p->size + p->deltaSize * dt);
				p->rotation = p->rotation + p->deltaRotation * dt;
				++it;
			}
			else
			{
				// 移除生命周期结束的粒子。
				ParticleMemory::freeParticle(*it);
				it = indexList->erase(it);
			}
		}
	}
}
