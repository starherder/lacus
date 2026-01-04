#pragma once
#include "particle_cfg.h"


namespace particle
{
	class ParticleMemory
	{
	public:
		static void initParticleMemory(int size);
		static void freeParticleMemory();

		static ParticleCfg* allocParticle();
		static void freeParticle(ParticleCfg* particle);

	private:
		static std::vector<ParticleCfg*> _particlePool;
		static std::vector<ParticleCfg*> _unusedParticleList;

		static int	_freeIndex;
		static bool _init;
	};
}