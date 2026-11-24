#include "particle_effect.h"
#include "particle_emitter.h"
#include "particle_manager.h"
#include "particle_memory.h"

#include <cmath>

namespace particle
{
	namespace
	{
		float applyCurve(ValueCurve curve, float t)
		{
			t = std::clamp(t, 0.0f, 1.0f);
			switch (curve)
			{
			case ValueCurve::EaseIn:
				return t * t;
			case ValueCurve::EaseOut:
				return 1.0f - (1.0f - t) * (1.0f - t);
			case ValueCurve::EaseInOut:
				return t < 0.5f ? 2.0f * t * t : 1.0f - std::pow(-2.0f * t + 2.0f, 2.0f) * 0.5f;
			case ValueCurve::SmoothStep:
				return t * t * (3.0f - 2.0f * t);
			case ValueCurve::Pulse:
				return std::sin(t * PI);
			case ValueCurve::Linear:
			default:
				return t;
			}
		}

		uint8_t lerpByte(uint8_t begin, uint8_t end, float t)
		{
			return static_cast<uint8_t>(std::clamp(begin + (end - begin) * t, 0.0f, 255.0f));
		}

		Color lerpColor(const Color& begin, const Color& end, float t)
		{
			return Color{
				lerpByte(begin.r, end.r, t),
				lerpByte(begin.g, end.g, t),
				lerpByte(begin.b, end.b, t),
				lerpByte(begin.a, end.a, t)
			};
		}

		uint8_t randByte(uint8_t value, uint8_t variance)
		{
			float v = value + variance * utility::rand_minus1_1();
			return static_cast<uint8_t>(std::clamp(v, 0.0f, 255.0f));
		}

		Color randomColor(const Color& color, const Color& variance)
		{
			return Color{
				randByte(color.r, variance.r),
				randByte(color.g, variance.g),
				randByte(color.b, variance.b),
				randByte(color.a, variance.a)
			};
		}

		void updateVisual(ParticleCfg* p, ValueCurve colorCurve, ValueCurve sizeCurve, ValueCurve spinCurve)
		{
			float progress = p->life > 0.0f ? p->elapsedLife / p->life : 1.0f;
			p->color = lerpColor(p->beginColor, p->endColor, applyCurve(colorCurve, progress));
			p->size = std::max(0.0f, p->beginSize + (p->endSize - p->beginSize) * applyCurve(sizeCurve, progress));
			p->rotation = p->beginRotation + (p->endRotation - p->beginRotation) * applyCurve(spinCurve, progress);
		}

		Vec2 calcWind(const ParticleCfg* p, const WindMode& wind)
		{
			if (wind.waveSpeed == 0.0f)
			{
				return p->windForce;
			}

			float wave = std::sin(p->elapsedLife * wind.waveSpeed + p->windPhase);
			return p->windForce + wind.wave * wave;
		}
	}

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
		_windMode = desc.windMode;
		_colorCurve = desc.colorCurve;
		_sizeCurve = desc.sizeCurve;
		_spinCurve = desc.spinCurve;
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
		particle->lastPos = particle->pos;
		particle->velocity = {0.0f, 0.0f};
		particle->windOffset = {0.0f, 0.0f};

		// 初始化粒子生命周期。
		particle->remainingLife = std::max(0.1f, _life + _lifeVar * utility::rand_minus1_1());
		particle->life = particle->remainingLife;
		particle->elapsedLife = 0.0f;

		// 初始化颜色渐变。
		Color begin_color = randomColor(_beginColor, _beginColorVar);
		Color end_color = randomColor(_endColor, _endColorVar);

		assert(particle->remainingLife!=0);

		float tmp = 1 / (particle->remainingLife);
		particle->beginColor = begin_color;
		particle->endColor = end_color;
		particle->color = begin_color;
		particle->deltaColor = (end_color - begin_color) * tmp;

		// 初始化大小渐变。
		float begin_size = std::max(0.0f, _beginSize + _beginSizeVar * utility::rand_minus1_1());
		float end_size = std::max(0.0f, _endSize + _endSizeVar * utility::rand_minus1_1());

		particle->beginSize = begin_size;
		particle->endSize = end_size;
		particle->size = begin_size;
		particle->deltaSize = (end_size - begin_size) / particle->remainingLife;

		// 初始化旋转渐变。
		float begin_spin = DegreeToRadian(std::max(0.0f, _beginSpin + _beginSpinVar * utility::rand_minus1_1()));
		float end_spin = DegreeToRadian(std::max(0.0f, _endSpin + _endSpinVar * utility::rand_minus1_1()));

		particle->beginRotation = begin_spin;
		particle->endRotation = end_spin;
		particle->rotation = begin_spin;
		particle->deltaRotation = (end_spin - begin_spin) / particle->remainingLife;
		particle->windForce = _windMode.force + Vec2{
			_windMode.forceVar.x * utility::rand_minus1_1(),
			_windMode.forceVar.y * utility::rand_minus1_1()
		};
		particle->windPhase = PI_2 * utility::rand_0_1();
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

		auto indexList = pe->getParticleList();
		for ( size_t i = 0; i < indexList->size(); )
		{
			ParticleCfg* p = (*indexList)[i];
			assert(p);
			if (!p)
			{
				(*indexList)[i] = indexList->back();
				indexList->pop_back();
				continue;
			}

			p->remainingLife -= dt;

			if ( p->remainingLife > 0 )
			{
				p->elapsedLife += dt;
				p->lastPos = p->pos;
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
				offset = (radial + tangential + _gravityMode.gravity + calcWind(p, _windMode)) * dt;

				p->gravityMode.initialVelocity = p->gravityMode.initialVelocity + offset;
				p->changePos = p->changePos + p->gravityMode.initialVelocity * dt;

				if ( _motionMode == MotionMode::Relative )
				{
					Vec2 diff = pe->getEmitPos() - p->startPos;
					p->pos = p->changePos + diff;
				}
				else
				{
					p->pos = p->changePos;
				}

				if (dt > 0.0f)
				{
					p->velocity = (p->pos - p->lastPos) / dt;
				}
				updateVisual(p, _colorCurve, _sizeCurve, _spinCurve);
				++i;
			}
			else
			{
				// 移除生命周期结束的粒子。
				pe->emitParticleDeath(p->pos);
				ParticleMemory::freeParticle(p);
				(*indexList)[i] = indexList->back();
				indexList->pop_back();
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

		auto indexList = pe->getParticleList();
		for ( size_t i = 0; i < indexList->size(); )
		{
			ParticleCfg* p = (*indexList)[i];
			assert(p);
			if (!p)
			{
				(*indexList)[i] = indexList->back();
				indexList->pop_back();
				continue;
			}

			p->remainingLife -= dt;

			if ( p->remainingLife > 0 )
			{
				p->elapsedLife += dt;
				p->lastPos = p->pos;
				p->radiusMode.angle += p->radiusMode.degreesPerSecond * dt;
				p->radiusMode.radius += p->radiusMode.deltaRadius * dt;

				p->changePos.x = cosf(p->radiusMode.angle) * p->radiusMode.radius;
				p->changePos.y = sinf(p->radiusMode.angle) * p->radiusMode.radius;
				p->windOffset = p->windOffset + calcWind(p, _windMode) * dt;
				
				if ( _motionMode == MotionMode::Free )
				{
					p->pos = p->changePos + p->windOffset + pe->getEmitPos();
				}
				else
				{
					p->pos = p->changePos + p->windOffset + p->startPos;
				}

				if (dt > 0.0f)
				{
					p->velocity = (p->pos - p->lastPos) / dt;
				}
				updateVisual(p, _colorCurve, _sizeCurve, _spinCurve);
				++i;
			}
			else
			{
				// 移除生命周期结束的粒子。
				pe->emitParticleDeath(p->pos);
				ParticleMemory::freeParticle(p);
				(*indexList)[i] = indexList->back();
				indexList->pop_back();
			}
		}
	}
}
