#include "particle_memory.h"

namespace particle
{
	std::vector<ParticleCfg*> ParticleMemory::_particlePool;
	std::vector<ParticleCfg*> ParticleMemory::_unusedParticleList;

	int  ParticleMemory::_freeIndex = 0;
	bool ParticleMemory::_init = false;


	void ParticleMemory::initParticleMemory(int size)
	{
		if ( _init ) return;
		_init = true;

		ParticleCfg* particle = nullptr;
		for ( int i = 0; i < size; i++ ) {
			particle = new ParticleCfg;
			if (particle) {
				_particlePool.push_back(particle);
				_unusedParticleList.push_back(particle);
			}
		}
	}

	void ParticleMemory::freeParticleMemory()
	{
		for ( auto& particle : _particlePool ) {
			delete particle;
		}
		_particlePool.clear();
		_unusedParticleList.clear();
	}

	ParticleCfg* ParticleMemory::allocParticle()
	{
		if ( (_freeIndex >= _particlePool.size() - 1) ) {
			return nullptr;
		}
		else {
			return _unusedParticleList[_freeIndex++];
		}
	}

	void ParticleMemory::freeParticle(ParticleCfg* particle)
	{
		assert(_freeIndex != 0);
		_unusedParticleList[--_freeIndex] = particle;
	}
}