#pragma once;



#include "ImForm.h"

#include "ParticleManager.h"
#include "../ParticleEditorCanvas.h"

namespace ore
{

	class ImFormParticle : public ImForm
	{
	public:
		ImFormParticle();
		~ImFormParticle();

	protected:
		void Draw();
		void Init();

		static bool GetParticleListItem(void*, int, const char**);	

		void ShowParticle(const char* name);
		void SaveParticle();
		void ReloadParticle();
		void StartParticle(bool start);

	private:
		std::vector<const char*> m_NameVector;

		int m_CurSelectIndex = 0;

		ParticleSharePtr m_CurParticle = nullptr;

		ParticleDescription m_ParticleDescript;

		ParticleEditorCanvas* m_ParticleEditorCanvas = nullptr;

		bool m_Modified = false;
	};

}