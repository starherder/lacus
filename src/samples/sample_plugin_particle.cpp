#include "sample_plugin_particle.h"

#include "engine/application.h"
#include "particle/particle_manager.h"

namespace samples 
{

	static auto setColorFloat = [](const Color& c, float* buf)
	{
		auto cc = c.toFColor();
		buf[0] = cc.r;
		buf[1] = cc.g;
		buf[2] = cc.b;
		buf[3] = cc.a;
	};

	static auto setFloatColor = [](float* buf)
	{
		return Color{ buf[0], buf[1], buf[2], buf[3] };
	};

	ImParticleEditor::ImParticleEditor(Particle* particle) : _particle(particle)
	{
		init();
	}

	ImParticleEditor::~ImParticleEditor()
	{
	}

	void ImParticleEditor::init()
	{
		auto& particles = ParticleManager::inst().GetAllParticleConfigs();
		for (auto& [name, file] : particles)
		{
			_nameVector.push_back(name.c_str());
		}
	}
	
	void ImParticleEditor::draw()
	{
		ImGui::Begin("Particles Editor");

		static int listbox_item_current = 0;
		if(ImGui::ListBox("particles##particle_list", &listbox_item_current, _nameVector.data(), (int)_nameVector.size()))
        {
            if(listbox_item_current != _selectIndex)
            {
                auto& sel_particle = _nameVector[listbox_item_current];
                on_particle_select.emit(sel_particle);

                _selectIndex = listbox_item_current;
            } 
        }

		if(_particle)
		{
			auto _particleDescript = _particle->Description();

			ImGui::Separator();
			
			char nameBuff[32] = { 0 };
			strcpy_s(nameBuff, 32, _particle->GetName().c_str());
			ImGui::LabelText("name", nameBuff);

			static std::string btnText = "Stop";
			if (ImGui::Button(btnText.c_str()))
			{
				bool clickStop = btnText == "Stop";
				btnText = clickStop ? "Start" : "Stop";

                if(clickStop) on_particle_stop.emit();
                else on_particle_start.emit();
			}

			ImGui::SameLine();

			if (ImGui::Button("Reload"))
			{
				on_particle_reload.emit();
			}

			ImGui::SameLine();

			if (ImGui::Button("Save"))
			{
				on_particle_save.emit();
			}

			char bufTexture[32] = { 0 };
			strcpy_s(bufTexture, 32, _particleDescript.sTexture.c_str());
			if (ImGui::InputText("texture", bufTexture, 32))
			{
				_particleDescript.sTexture = bufTexture;
				_modified = true;
			}

			float emitPos[2] = { _particleDescript.vEmitPos.x, _particleDescript.vEmitPos.y };
			if (ImGui::InputFloat2("emit pos", emitPos))
			{
				_particleDescript.vEmitPos = { emitPos[0], emitPos[1] };
				_modified = true;
			}

			float emitPosVar[2] = { _particleDescript.vEmitPosVar.x, _particleDescript.vEmitPosVar.y };
			if (ImGui::InputFloat2("emit pos var", emitPosVar))
			{
				_particleDescript.vEmitPosVar = { emitPosVar[0], emitPosVar[1] };
				_modified = true;
			}

			if (ImGui::InputFloat("angle", &_particleDescript.fEmitAngle))
			{
				_modified = true;
			}

			if (ImGui::InputFloat("angle var", &_particleDescript.fEmitAngleVar))
			{
				_modified = true;
			}

			if (ImGui::InputFloat("speed", &_particleDescript.fEmitSpeed))
			{
				_modified = true;
			}

			if (ImGui::InputFloat("speed var", &_particleDescript.fEmitSpeedVar))
			{
				_modified = true;
			}

			if (ImGui::InputInt("particle count", &_particleDescript.nParticleCount))
			{
				_modified = true;
			}

			if (ImGui::InputFloat("duration", &_particleDescript.fDuration))
			{
				_modified = true;
			}

			int motionMode = (int)_particleDescript.motionMode;
			if (ImGui::RadioButton("MOTION_MODE_FREE", &motionMode, 0))
			{
				_particleDescript.motionMode = (MotionMode)motionMode;
				_modified = true;
			}

			ImGui::SameLine();

			if (ImGui::RadioButton("MOTION_MODE_RELATIVE", &motionMode, 1))
			{
				_particleDescript.motionMode = (MotionMode)motionMode;
				_modified = true;
			}

			int emitType = (int)_particleDescript.emitterType;
			if (ImGui::RadioButton("EMITTER_TYPE_GRAVITY", &emitType, 0))
			{
				_particleDescript.emitterType = (EmitterType)emitType;
				_modified = true;
			}

			ImGui::SameLine();

			if (ImGui::RadioButton("EMITTER_TYPE_RADIUS", &emitType, 1))
			{
				_particleDescript.emitterType = (EmitterType)emitType;
				_modified = true;
			}

			if (emitType == (int)EmitterType::EMITTER_TYPE_GRAVITY)
			{
				float grav[2] = { _particleDescript.gravityMode.vGravity.x, _particleDescript.gravityMode.vGravity.x };
				if (ImGui::InputFloat2("gravity", grav))
				{
					_particleDescript.gravityMode.vGravity = { grav[0], grav[1] };
					_modified = true;
				}

				if (ImGui::InputFloat("tangent accel", &_particleDescript.gravityMode.fTangentialAccel))
				{
					_modified = true;
				}
				if (ImGui::InputFloat("tangent accel var", &_particleDescript.gravityMode.fTangentialAccelVar))
				{
					_modified = true;
				}

				if (ImGui::InputFloat("Radio accel", &_particleDescript.gravityMode.fRadialAccel))
				{
					_modified = true;
				}
				if (ImGui::InputFloat("Radio accel var", &_particleDescript.gravityMode.fRadialAccelVar))
				{
					_modified = true;
				}
			}
			else if (emitType == (int)EmitterType::EMITTER_TYPE_RADIUS)
			{
				if (ImGui::InputFloat("begin radius", &_particleDescript.radiusMode.fBeginRadius))
				{
					_modified = true;
				}
				if (ImGui::InputFloat("begin radius var", &_particleDescript.radiusMode.fBeginRadiusVar))
				{
					_modified = true;
				}

				if (ImGui::InputFloat("end radius", &_particleDescript.radiusMode.fEndRadius))
				{
					_modified = true;
				}
				if (ImGui::InputFloat("end radius var", &_particleDescript.radiusMode.fEndRadiusVar))
				{
					_modified = true;
				}

				if (ImGui::InputFloat("spin speed", &_particleDescript.radiusMode.fSpinPerSecond))
				{
					_modified = true;
				}
				if (ImGui::InputFloat("spin speed var", &_particleDescript.radiusMode.fSpinPerSecondVar))
				{
					_modified = true;
				}
			}

			if (ImGui::InputFloat("life", &_particleDescript.fLife))
			{
				_modified = true;
			}

			if (ImGui::InputFloat("life var", &_particleDescript.fLifeVar))
			{
				_modified = true;
			}

			float size[2] = { _particleDescript.fBeginSize, _particleDescript.fEndSize };
			if (ImGui::InputFloat2("BeginEnd size", size))
			{
				_particleDescript.fBeginSize = size[0];
				_particleDescript.fEndSize = size[1];
				_modified = true;
			}

			float sizeVar[2] = { _particleDescript.fBeginSizeVar, _particleDescript.fEndSizeVar };
			if (ImGui::InputFloat2("BeginEnd size var", sizeVar))
			{
				_particleDescript.fBeginSizeVar = sizeVar[0];
				_particleDescript.fEndSizeVar = sizeVar[1];
				_modified = true;
			}

			float spin[2] = { _particleDescript.fBeginSpin, _particleDescript.fEndSpin };
			if (ImGui::InputFloat2("BeginEnd spin", size))
			{
				_particleDescript.fBeginSpin = spin[0];
				_particleDescript.fEndSpin = spin[1];
				_modified = true;
			}

			float spinVar[2] = { _particleDescript.fBeginSpinVar, _particleDescript.fEndSpinVar };
			if (ImGui::InputFloat2("BeginEnd spin var", spinVar))
			{
				_particleDescript.fBeginSpinVar = spinVar[0];
				_particleDescript.fEndSpinVar = spinVar[1];
				_modified = true;
			}

			float begColor[4] = { 0 };
			setColorFloat(_particleDescript.cBeginColor, begColor);
			if (ImGui::ColorEdit4("Begin Color", begColor))
			{
				_particleDescript.cBeginColor = setFloatColor(begColor);
				_modified = true;
			}

			float begColorVar[4] = { 0 };
			setColorFloat(_particleDescript.cBeginColorVar, begColorVar);
			if (ImGui::ColorEdit4("Begin Color Var", begColorVar))
			{
				_particleDescript.cBeginColorVar = setFloatColor(begColorVar);
				_modified = true;
			}

			float endColor[4] = { 0 };
			setColorFloat(_particleDescript.cEndColor, endColor);
			if (ImGui::ColorEdit4("End Color", endColor))
			{
				_particleDescript.cEndColor = setFloatColor(endColor);
				_modified = true;
			}

			float endColorVar[4] = { 0 };
			setColorFloat(_particleDescript.cEndColorVar, endColorVar);
			if (ImGui::ColorEdit4("End Color Var", endColorVar))
			{
				_particleDescript.cEndColorVar = setFloatColor(endColorVar);
				_modified = true;
			}

			if (_modified)
			{
                _particle->CorrectDescription(_particleDescript);
                _particle->SetDescription(_particleDescript);
                
				_modified = false;
			}
		}

		ImGui::End();
	}

    /////////////////////////////////////////////////////////////////////////////////////////////

    void SamplePluginParticle::onInit() 
    {
        ParticleManager::inst().init(application());

        bool res = ParticleManager::inst().LoadParticles("particles/particles.xml");
        if (!res) {
            spdlog::error("load particles failed.");
            return;
        }
    }

    void SamplePluginParticle::onInstall() 
    {  
    }

    void SamplePluginParticle::onUninstall() 
    {
    }

    void SamplePluginParticle::onEnable()
    {
        auto form = imgui::ImFormManager::inst().showForm<ImParticleEditor>("particle_editor", _particle.get());
        form->on_particle_select.connect(this, &SamplePluginParticle::onParticleSelect);
        form->on_particle_start.connect(this, &SamplePluginParticle::onParticleStart);
        form->on_particle_stop.connect(this, &SamplePluginParticle::onParticleStop);
        form->on_particle_save.connect(this, &SamplePluginParticle::onParticleSave);
        form->on_particle_reload.connect(this, &SamplePluginParticle::onParticleReload);
    }

    void SamplePluginParticle::onDisable()
    {
        imgui::ImFormManager::inst().closeForm("particle_editor");
    }

    void SamplePluginParticle::onUpdate() 
    {
        if (_particle) 
        {
            _particle->Update(application()->frameTicker().deltaSeconds());
        }
    }

    void SamplePluginParticle::onDraw() 
    {
        if (_particle) 
        {
            _particle->Draw();
        }
    }

    void SamplePluginParticle::onClose() 
    {
    }

    void SamplePluginParticle::onParticleSelect(const char* particle)
    {
        _particle = particle::ParticleManager::inst().CreateParticle(particle);
        if (!_particle)
        {
            spdlog::error("create particle ({}) failed.", particle);
            return;
        }

        _particle->SetPos({ 500, 500 });
        _particle->Start();

        auto form = imgui::ImFormManager::inst().getForm<ImParticleEditor>("particle_editor");
        if(form)
        {
            form->setParticle(_particle.get());
        }
    }
    
    void SamplePluginParticle::onParticleStart()
    {
        if (_particle)
        {
            _particle->Start();
        }
    }
    
    void SamplePluginParticle::onParticleStop()
    {
        if (_particle)
        {
            _particle->Stop();
        }
    }
    
    void SamplePluginParticle::onParticleSave()
    {
        if (_particle)
        {
            _particle->Save();
        }
    }

    void SamplePluginParticle::onParticleReload()
    {
        if(_particle)
        {
            onParticleSelect(_particle->GetName().c_str());
        }
    }
}