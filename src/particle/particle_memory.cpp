#include "particle_memory.h"

namespace particle
{
	std::vector<ParticleCfg*> ParticleMemory::_particlePool;
	std::vector<ParticleCfg*> ParticleMemory::_unusedParticleList;

	int  ParticleMemory::_freeIndex = 0;
	bool ParticleMemory::_init = false;
	int ParticleMemory::_growSize = 1024;


	void ParticleMemory::initParticleMemory(int size)

	{
		if ( _init ) return;
		_init = true;
		_growSize = std::max(256, size / 4);

		reserveParticleMemory(size);
	}

	void ParticleMemory::freeParticleMemory()
	{
		for ( auto& particle : _particlePool )
		{
			delete particle;
		}
		_particlePool.clear();
		_unusedParticleList.clear();
		_freeIndex = 0;
		_init = false;
	}

	void ParticleMemory::reserveParticleMemory(int size)
	{
		int needSize = size - static_cast<int>(_particlePool.size());
		for ( int i = 0; i < needSize; i++ )
		{
			ParticleCfg* particle = createParticle();
			if (particle)
			{
				_particlePool.push_back(particle);
				_unusedParticleList.push_back(particle);
			}
		}
	}

	ParticleCfg* ParticleMemory::allocParticle()
	{
		if ( _freeIndex >= static_cast<int>(_unusedParticleList.size()) )
		{
			int oldSize = static_cast<int>(_particlePool.size());
			reserveParticleMemory(oldSize + _growSize);
			LogWarn("ParticleMemory: pool expanded from {} to {}.", oldSize, _particlePool.size());

			if ( _freeIndex >= static_cast<int>(_unusedParticleList.size()) )
			{
				LogWarn("ParticleMemory: alloc particle failed, pool size = {}.", _particlePool.size());
				return nullptr;
			}
		}

		return _unusedParticleList[_freeIndex++];
	}

	void ParticleMemory::freeParticle(ParticleCfg* particle)
	{
		assert(_freeIndex != 0);
		_unusedParticleList[--_freeIndex] = particle;
	}

	ParticleCfg* ParticleMemory::createParticle()
	{
		return new ParticleCfg;
	}
}
