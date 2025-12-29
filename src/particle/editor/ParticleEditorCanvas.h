#pragma once
#include "Interface.h"
#include "ParticleManager.h"

namespace ore
{



	class ParticleEditorCanvas :	public IDrawable, 
									public IUpdateable,
									public sigslot::SigSlotBase,
									public ISingleton<ParticleEditorCanvas>
	{
	public:
		ParticleEditorCanvas();
		~ParticleEditorCanvas();

		void Update(float seconds);
		void Draw(float seconds);

		void SetParticle(ParticleSharePtr ptr);

	private:
		ParticleSharePtr m_Particle;		
	};


}