#pragma once
#include "ParticleCfg.h"

#include <vector>

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
		static std::vector<ParticleCfg*> vParticlePool;
		static std::vector<ParticleCfg*> vUnusedParticleList;

		static int	nFreeIndex;
		static bool bInit;
	};
}