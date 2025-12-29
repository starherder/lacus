#include "ParticleMemory.h"

namespace particle
{
	std::vector<ParticleCfg*> ParticleMemory::vParticlePool;
	std::vector<ParticleCfg*> ParticleMemory::vUnusedParticleList;

	int  ParticleMemory::nFreeIndex = 0;
	bool ParticleMemory::bInit = false;


	void ParticleMemory::initParticleMemory(int size)
	{
		if ( bInit ) return;
		bInit = true;

		ParticleCfg* particle = nullptr;
		for ( int i = 0; i < size; i++ ) {
			particle = new ParticleCfg;
			if (particle) {
				vParticlePool.push_back(particle);
				vUnusedParticleList.push_back(particle);
			}
		}
	}

	void ParticleMemory::freeParticleMemory()
	{
		for ( auto& particle : vParticlePool ) {
			delete particle;
		}
		vParticlePool.clear();
		vUnusedParticleList.clear();
	}

	ParticleCfg* ParticleMemory::allocParticle()
	{
		if ( (nFreeIndex >= vParticlePool.size() - 1) ) {
			return nullptr;
		}
		else {
			return vUnusedParticleList[nFreeIndex++];
		}
	}

	void ParticleMemory::freeParticle(ParticleCfg* particle)
	{
		assert(nFreeIndex != 0);
		vUnusedParticleList[--nFreeIndex] = particle;
	}
}