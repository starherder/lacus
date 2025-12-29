#include "ImFormParticle.h"
#include "../ParticleEditorCanvas.h"

namespace ore
{

	static auto setColorFloat = [](const Color4f& c, float* buf)
	{
		buf[0] = c.R;
		buf[1] = c.G;
		buf[2] = c.B;
		buf[3] = c.A;
	};

	static auto setFloatColor = [](float* buf)
	{
		Color4f c;
		c.Set(buf[0], buf[1], buf[2], buf[3]);
		return c;
	};

	ImFormParticle::ImFormParticle()
	{
		Init();
	}

	ImFormParticle::~ImFormParticle()
	{
	}


	void ImFormParticle::Init()
	{
		ParticleManager::InstancePtr()->LoadParticles("particles.xml");

		auto& particles = ParticleManager::InstancePtr()->GetAllParticleCfg();
		for (auto& it : particles)
		{
			m_NameVector.push_back(it.first.c_str());
		}

		m_ParticleEditorCanvas = ParticleEditorCanvas::InstancePtr();

		Engine::InstancePtr()->AddDrawable(m_ParticleEditorCanvas);
		Engine::InstancePtr()->AddUpdateable(m_ParticleEditorCanvas);
	}
	
	void ImFormParticle::Draw()
	{
		if (!ImGui::Begin("Particles Editor", &m_bVisible, 0))
		{
			ImGui::End();
			return;
		}

		// 粒子列表
		static int listbox_item_current = 0;
		ImGui::ListBox("particles", &listbox_item_current, &ImFormParticle::GetParticleListItem, this, (int)m_NameVector.size());
		if(listbox_item_current != m_CurSelectIndex)
		{
			const char* name = nullptr;
			GetParticleListItem(this, listbox_item_current, &name);
			
			ShowParticle(name);

			m_CurSelectIndex = listbox_item_current;
		}

		if(m_CurParticle)
		{
			m_ParticleDescript = m_CurParticle->Description();

			ImGui::Separator();

			// 粒子属性
			
			char nameBuff[32] = { 0 };
			strcpy(nameBuff, m_CurParticle->GetName().c_str());
			ImGui::LabelText("name", nameBuff);

			static std::string btnText = "Stop";
			if (ImGui::Button(btnText.c_str()))
			{
				bool clickStop = btnText == "Stop";
				btnText = clickStop ? "Start" : "Stop";

				StartParticle(!clickStop);
			}

			ImGui::SameLine();

			if (ImGui::Button("Reload"))
			{
				ReloadParticle();
			}

			ImGui::SameLine();

			if (ImGui::Button("Save"))
			{
				SaveParticle();
			}

			char bufTexture[32] = { 0 };
			strcpy(bufTexture, m_ParticleDescript.sTexture.c_str());
			if (ImGui::InputText("texture", bufTexture, 32))
			{
				m_ParticleDescript.sTexture = bufTexture;
				m_Modified = true;
			}

			float emitPos[2] = { m_ParticleDescript.vEmitPos.x, m_ParticleDescript.vEmitPos.y };
			if (ImGui::InputFloat2("emit pos", emitPos))
			{
				m_ParticleDescript.vEmitPos = { emitPos[0], emitPos[1] };
				m_Modified = true;
			}

			float emitPosVar[2] = { m_ParticleDescript.vEmitPosVar.x, m_ParticleDescript.vEmitPosVar.y };
			if (ImGui::InputFloat2("emit pos var", emitPosVar))
			{
				m_ParticleDescript.vEmitPosVar = { emitPosVar[0], emitPosVar[1] };
				m_Modified = true;
			}

			if (ImGui::InputFloat("angle", &m_ParticleDescript.fEmitAngle))
			{
				m_Modified = true;
			}

			if (ImGui::InputFloat("angle var", &m_ParticleDescript.fEmitAngleVar))
			{
				m_Modified = true;
			}

			if (ImGui::InputFloat("speed", &m_ParticleDescript.fEmitSpeed))
			{
				m_Modified = true;
			}

			if (ImGui::InputFloat("speed var", &m_ParticleDescript.fEmitSpeedVar))
			{
				m_Modified = true;
			}

			if (ImGui::InputInt("particle count", &m_ParticleDescript.nParticleCount))
			{
				m_Modified = true;
			}

			if (ImGui::InputFloat("duration", &m_ParticleDescript.fDuration))
			{
				m_Modified = true;
			}

			int motionMode = (int)m_ParticleDescript.motionMode;
			if (ImGui::RadioButton("MOTION_MODE_FREE", &motionMode, 0))
			{
				m_ParticleDescript.motionMode = (MotionMode)motionMode;
				m_Modified = true;
			}

			ImGui::SameLine();

			if (ImGui::RadioButton("MOTION_MODE_RELATIVE", &motionMode, 1))
			{
				m_ParticleDescript.motionMode = (MotionMode)motionMode;
				m_Modified = true;
			}

			int emitType = (int)m_ParticleDescript.emitterType;
			if (ImGui::RadioButton("EMITTER_TYPE_GRAVITY", &emitType, 0))
			{
				m_ParticleDescript.emitterType = (EmitterType)emitType;
				m_Modified = true;
			}

			ImGui::SameLine();

			if (ImGui::RadioButton("EMITTER_TYPE_RADIUS", &emitType, 1))
			{
				m_ParticleDescript.emitterType = (EmitterType)emitType;
				m_Modified = true;
			}

			if (emitType == (int)EmitterType::EMITTER_TYPE_GRAVITY)
			{
				float grav[2] = { m_ParticleDescript.gravityMode.vGravity.x, m_ParticleDescript.gravityMode.vGravity.x };
				if (ImGui::InputFloat2("gravity", grav))
				{
					m_ParticleDescript.gravityMode.vGravity = { grav[0], grav[1] };
					m_Modified = true;
				}

				if (ImGui::InputFloat("tangent accel", &m_ParticleDescript.gravityMode.fTangentialAccel))
				{
					m_Modified = true;
				}
				if (ImGui::InputFloat("tangent accel var", &m_ParticleDescript.gravityMode.fTangentialAccelVar))
				{
					m_Modified = true;
				}

				if (ImGui::InputFloat("Radio accel", &m_ParticleDescript.gravityMode.fRadialAccel))
				{
					m_Modified = true;
				}
				if (ImGui::InputFloat("Radio accel var", &m_ParticleDescript.gravityMode.fRadialAccelVar))
				{
					m_Modified = true;
				}
			}
			else if (emitType == (int)EmitterType::EMITTER_TYPE_RADIUS)
			{
				if (ImGui::InputFloat("begin radius", &m_ParticleDescript.radiusMode.fBeginRadius))
				{
					m_Modified = true;
				}
				if (ImGui::InputFloat("begin radius var", &m_ParticleDescript.radiusMode.fBeginRadiusVar))
				{
					m_Modified = true;
				}

				if (ImGui::InputFloat("end radius", &m_ParticleDescript.radiusMode.fEndRadius))
				{
					m_Modified = true;
				}
				if (ImGui::InputFloat("end radius var", &m_ParticleDescript.radiusMode.fEndRadiusVar))
				{
					m_Modified = true;
				}

				if (ImGui::InputFloat("spin speed", &m_ParticleDescript.radiusMode.fSpinPerSecond))
				{
					m_Modified = true;
				}
				if (ImGui::InputFloat("spin speed var", &m_ParticleDescript.radiusMode.fSpinPerSecondVar))
				{
					m_Modified = true;
				}
			}

			if (ImGui::InputFloat("life", &m_ParticleDescript.fLife))
			{
				m_Modified = true;
			}

			if (ImGui::InputFloat("life var", &m_ParticleDescript.fLifeVar))
			{
				m_Modified = true;
			}

			float size[2] = { m_ParticleDescript.fBeginSize, m_ParticleDescript.fEndSize };
			if (ImGui::InputFloat2("BeginEnd size", size))
			{
				m_ParticleDescript.fBeginSize = size[0];
				m_ParticleDescript.fEndSize = size[1];
				m_Modified = true;
			}

			float sizeVar[2] = { m_ParticleDescript.fBeginSizeVar, m_ParticleDescript.fEndSizeVar };
			if (ImGui::InputFloat2("BeginEnd size var", sizeVar))
			{
				m_ParticleDescript.fBeginSizeVar = sizeVar[0];
				m_ParticleDescript.fEndSizeVar = sizeVar[1];
				m_Modified = true;
			}

			float spin[2] = { m_ParticleDescript.fBeginSpin, m_ParticleDescript.fEndSpin };
			if (ImGui::InputFloat2("BeginEnd spin", size))
			{
				m_ParticleDescript.fBeginSpin = spin[0];
				m_ParticleDescript.fEndSpin = spin[1];
				m_Modified = true;
			}

			float spinVar[2] = { m_ParticleDescript.fBeginSpinVar, m_ParticleDescript.fEndSpinVar };
			if (ImGui::InputFloat2("BeginEnd spin var", spinVar))
			{
				m_ParticleDescript.fBeginSpinVar = spinVar[0];
				m_ParticleDescript.fEndSpinVar = spinVar[1];
				m_Modified = true;
			}

			float begColor[4] = { 0 };
			setColorFloat(m_ParticleDescript.cBeginColor, begColor);
			if (ImGui::ColorEdit4("Begin Color", begColor))
			{
				m_ParticleDescript.cBeginColor = setFloatColor(begColor);
				m_Modified = true;
			}

			float begColorVar[4] = { 0 };
			setColorFloat(m_ParticleDescript.cBeginColorVar, begColorVar);
			if (ImGui::ColorEdit4("Begin Color Var", begColorVar))
			{
				m_ParticleDescript.cBeginColorVar = setFloatColor(begColorVar);
				m_Modified = true;
			}

			float endColor[4] = { 0 };
			setColorFloat(m_ParticleDescript.cEndColor, endColor);
			if (ImGui::ColorEdit4("End Color", endColor))
			{
				m_ParticleDescript.cEndColor = setFloatColor(endColor);
				m_Modified = true;
			}


			float endColorVar[4] = { 0 };
			setColorFloat(m_ParticleDescript.cEndColorVar, endColorVar);
			if (ImGui::ColorEdit4("End Color Var", endColorVar))
			{
				m_ParticleDescript.cEndColorVar = setFloatColor(endColorVar);
				m_Modified = true;
			}

			if (m_Modified)
			{
				m_CurParticle->CorrectDescription(m_ParticleDescript);
				m_CurParticle->SetDescription(m_ParticleDescript);

				m_Modified = false;
			}
			
			m_CurParticle->Draw();
		}

		ImGui::End();
	}

	void ImFormParticle::ShowParticle(const char* name)
	{
		fail_return(name);
		if (m_CurParticle && m_CurParticle->IsPlaying())
		{
			m_CurParticle->Stop();
		}

		m_CurParticle = ParticleManager::InstancePtr()->CreateParticle(name);
		if (m_CurParticle)
		{
			m_CurParticle->Start();
			if (m_ParticleEditorCanvas)
			{
				m_ParticleEditorCanvas->SetParticle(m_CurParticle);
			}
		}
	}

	bool ImFormParticle::GetParticleListItem(void* data, int index, const char** text)
	{
		ImFormParticle* pThis = (ImFormParticle*)data;
		fail_return_false(pThis);

		*text = pThis->m_NameVector[index];

		return true;
	}

	void ImFormParticle::StartParticle(bool start)
	{
		if (m_CurParticle)
		{
			if (start)
			{
				m_CurParticle->Start();
			}
			else
			{
				m_CurParticle->Stop();
			}
		}
	}
	
	void ImFormParticle::ReloadParticle()
	{
		if (m_CurParticle)
		{
			m_CurParticle = ParticleManager::InstancePtr()->CreateParticle(m_CurParticle->GetName());
			m_ParticleDescript = m_CurParticle->Description();
		}
	}
	
	void ImFormParticle::SaveParticle()
	{
		if (m_CurParticle)
		{
			m_CurParticle->Save();
		}
	}

}