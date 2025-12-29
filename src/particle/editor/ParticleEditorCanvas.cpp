#include "ParticleEditorCanvas.h"



namespace ore
{


	ParticleEditorCanvas::ParticleEditorCanvas()
	{

	}
	
	ParticleEditorCanvas::~ParticleEditorCanvas()
	{

	}

	void ParticleEditorCanvas::SetParticle(ParticleSharePtr ptr)
	{
		m_Particle = ptr;
	}
	
	void ParticleEditorCanvas::Update(float seconds)
	{
		if (m_Particle)
		{
			m_Particle->Update(seconds);
		}

	}
	void ParticleEditorCanvas::Draw(float seconds)
	{
		if (m_Particle)
		{
			m_Particle->Draw();
		}
	}


}