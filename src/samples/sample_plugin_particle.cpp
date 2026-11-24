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
		auto& particles = ParticleManager::inst().getAllParticleConfigs();
		for (auto& [name, file] : particles)
		{
			_nameVector.push_back(name.c_str());
		}
	}
	
	void ImParticleEditor::draw()
	{
		ImGui::Begin("Particles Editor");

		static int listbox_item_current = 0;
		ImGui::SetNextWindowSize(ImVec2(-FLT_MIN, 400.0f));
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
			auto _particleDescript = _particle->description();

			ImGui::Separator();
			
			char nameBuff[32] = { 0 };
			strcpy_s(nameBuff, 32, _particle->getName().c_str());
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
			strcpy_s(bufTexture, 32, _particleDescript.texture.c_str());
			if (ImGui::InputText("texture", bufTexture, 32))
			{
				_particleDescript.texture = bufTexture;
				_modified = true;
			}

			float emitPos[2] = { _particleDescript.emitPos.x, _particleDescript.emitPos.y };
			if (ImGui::InputFloat2("emit pos", emitPos))
			{
				_particleDescript.emitPos = { emitPos[0], emitPos[1] };
				_modified = true;
			}

			float emitPosVar[2] = { _particleDescript.emitPosVar.x, _particleDescript.emitPosVar.y };
			if (ImGui::InputFloat2("emit pos var", emitPosVar))
			{
				_particleDescript.emitPosVar = { emitPosVar[0], emitPosVar[1] };
				if (_particleDescript.emitterShape == EmitterShape::Rect)
				{
					_particleDescript.emitShapeSize = _particleDescript.emitPosVar * 2.0f;
				}
				_modified = true;
			}

			int emitShape = (int)_particleDescript.emitterShape;
			if (ImGui::RadioButton("Point", &emitShape, 0))
			{
				_particleDescript.emitterShape = (EmitterShape)emitShape;
				_modified = true;
			}

			ImGui::SameLine();

			if (ImGui::RadioButton("Line", &emitShape, 1))
			{
				_particleDescript.emitterShape = (EmitterShape)emitShape;
				_modified = true;
			}

			if (ImGui::RadioButton("Rect", &emitShape, 2))
			{
				_particleDescript.emitterShape = (EmitterShape)emitShape;
				_modified = true;
			}

			ImGui::SameLine();

			if (ImGui::RadioButton("Circle", &emitShape, 3))
			{
				_particleDescript.emitterShape = (EmitterShape)emitShape;
				_modified = true;
			}

			float emitShapeSize[2] = { _particleDescript.emitShapeSize.x, _particleDescript.emitShapeSize.y };
			if (ImGui::InputFloat2("emit shape size", emitShapeSize))
			{
				_particleDescript.emitShapeSize = { emitShapeSize[0], emitShapeSize[1] };
				_particleDescript.emitPosVar = _particleDescript.emitShapeSize * 0.5f;
				_modified = true;
			}

			if (ImGui::InputFloat("emit shape radius", &_particleDescript.emitShapeRadius))
			{
				_modified = true;
			}

			if (ImGui::InputFloat("emit shape rotation", &_particleDescript.emitShapeRotation))
			{
				_modified = true;
			}

			if (ImGui::Checkbox("emit shape edge only", &_particleDescript.emitShapeEdgeOnly))
			{
				_modified = true;
			}

			if (ImGui::InputFloat("angle", &_particleDescript.emitAngle))
			{
				_modified = true;
			}

			if (ImGui::InputFloat("angle var", &_particleDescript.emitAngleVar))
			{
				_modified = true;
			}

			if (ImGui::InputFloat("speed", &_particleDescript.emitSpeed))
			{
				_modified = true;
			}

			if (ImGui::InputFloat("speed var", &_particleDescript.emitSpeedVar))
			{
				_modified = true;
			}

			if (ImGui::InputInt("particle count", &_particleDescript.particleCount))
			{
				_modified = true;
			}

			if (ImGui::InputFloat("duration", &_particleDescript.duration))
			{
				_modified = true;
			}

			int motionMode = (int)_particleDescript.motionMode;
			if (ImGui::RadioButton("Free", &motionMode, 0))
			{
				_particleDescript.motionMode = (MotionMode)motionMode;
				_modified = true;
			}

			ImGui::SameLine();

			if (ImGui::RadioButton("Relative", &motionMode, 1))
			{
				_particleDescript.motionMode = (MotionMode)motionMode;
				_modified = true;
			}

			int emitType = (int)_particleDescript.emitterType;
			if (ImGui::RadioButton("Gravity", &emitType, 0))
			{
				_particleDescript.emitterType = (EmitterType)emitType;
				_modified = true;
			}

			ImGui::SameLine();

			if (ImGui::RadioButton("Radius", &emitType, 1))
			{
				_particleDescript.emitterType = (EmitterType)emitType;
				_modified = true;
			}

			if (emitType == (int)EmitterType::Gravity)
			{
				float grav[2] = { _particleDescript.gravityMode.gravity.x, _particleDescript.gravityMode.gravity.y };
				if (ImGui::InputFloat2("gravity", grav))
				{
					_particleDescript.gravityMode.gravity = { grav[0], grav[1] };
					_modified = true;
				}

				if (ImGui::InputFloat("tangent accel", &_particleDescript.gravityMode.tangentialAccel))
				{
					_modified = true;
				}
				if (ImGui::InputFloat("tangent accel var", &_particleDescript.gravityMode.tangentialAccelVar))
				{
					_modified = true;
				}

				if (ImGui::InputFloat("Radio accel", &_particleDescript.gravityMode.radialAccel))
				{
					_modified = true;
				}
				if (ImGui::InputFloat("Radio accel var", &_particleDescript.gravityMode.radialAccelVar))
				{
					_modified = true;
				}
			}
			else if (emitType == (int)EmitterType::Radius)
			{
				if (ImGui::InputFloat("begin radius", &_particleDescript.radiusMode.beginRadius))
				{
					_modified = true;
				}
				if (ImGui::InputFloat("begin radius var", &_particleDescript.radiusMode.beginRadiusVar))
				{
					_modified = true;
				}

				if (ImGui::InputFloat("end radius", &_particleDescript.radiusMode.endRadius))
				{
					_modified = true;
				}
				if (ImGui::InputFloat("end radius var", &_particleDescript.radiusMode.endRadiusVar))
				{
					_modified = true;
				}

				if (ImGui::InputFloat("spin speed", &_particleDescript.radiusMode.spinPerSecond))
				{
					_modified = true;
				}
				if (ImGui::InputFloat("spin speed var", &_particleDescript.radiusMode.spinPerSecondVar))
				{
					_modified = true;
				}
			}

			if (ImGui::InputFloat("life", &_particleDescript.life))
			{
				_modified = true;
			}

			if (ImGui::InputFloat("life var", &_particleDescript.lifeVar))
			{
				_modified = true;
			}

			float size[2] = { _particleDescript.beginSize, _particleDescript.endSize };
			if (ImGui::InputFloat2("BeginEnd size", size))
			{
				_particleDescript.beginSize = size[0];
				_particleDescript.endSize = size[1];
				_modified = true;
			}

			float sizeVar[2] = { _particleDescript.beginSizeVar, _particleDescript.endSizeVar };
			if (ImGui::InputFloat2("BeginEnd size var", sizeVar))
			{
				_particleDescript.beginSizeVar = sizeVar[0];
				_particleDescript.endSizeVar = sizeVar[1];
				_modified = true;
			}

			float spin[2] = { _particleDescript.beginSpin, _particleDescript.endSpin };
			if (ImGui::InputFloat2("BeginEnd spin", spin))
			{
				_particleDescript.beginSpin = spin[0];
				_particleDescript.endSpin = spin[1];
				_modified = true;
			}

			float spinVar[2] = { _particleDescript.beginSpinVar, _particleDescript.endSpinVar };
			if (ImGui::InputFloat2("BeginEnd spin var", spinVar))
			{
				_particleDescript.beginSpinVar = spinVar[0];
				_particleDescript.endSpinVar = spinVar[1];
				_modified = true;
			}

			float begColor[4] = { 0 };
			setColorFloat(_particleDescript.beginColor, begColor);
			if (ImGui::ColorEdit4("Begin Color", begColor))
			{
				_particleDescript.beginColor = setFloatColor(begColor);
				_modified = true;
			}

			float begColorVar[4] = { 0 };
			setColorFloat(_particleDescript.beginColorVar, begColorVar);
			if (ImGui::ColorEdit4("Begin Color Var", begColorVar))
			{
				_particleDescript.beginColorVar = setFloatColor(begColorVar);
				_modified = true;
			}

			float endColor[4] = { 0 };
			setColorFloat(_particleDescript.endColor, endColor);
			if (ImGui::ColorEdit4("End Color", endColor))
			{
				_particleDescript.endColor = setFloatColor(endColor);
				_modified = true;
			}

			float endColorVar[4] = { 0 };
			setColorFloat(_particleDescript.endColorVar, endColorVar);
			if (ImGui::ColorEdit4("End Color Var", endColorVar))
			{
				_particleDescript.endColorVar = setFloatColor(endColorVar);
				_modified = true;
			}

			if (_modified)
			{
                _particle->correctDescription(_particleDescript);
                _particle->setDescription(_particleDescript);
                
				_modified = false;
			}
		}

		ImGui::End();
	}

    /////////////////////////////////////////////////////////////////////////////////////////////

    void SamplePluginParticle::onInit() 
    {
        ParticleManager::inst().init(application());

		auto cfgfile = application()->resPath() / "particles/";

        bool res = ParticleManager::inst().loadParticles(cfgfile);
        if (!res) {
            LogError("load particles failed.");
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

		application()->eventDispatcher().onMouseLeftDrag.connect(this, &SamplePluginParticle::onMouseDrag);
    }

    void SamplePluginParticle::onDisable()
    {
        imgui::ImFormManager::inst().closeForm("particle_editor");
    }

    void SamplePluginParticle::onUpdate() 
    {
        if (_particle) 
        {
            _particle->update(application()->frameTicker().deltaSeconds());
        }
    }

    void SamplePluginParticle::onDraw() 
    {
        if (_particle) 
        {
            _particle->draw();
        }
    }

    void SamplePluginParticle::onClose() 
    {
    }

	void SamplePluginParticle::onMouseDrag(const Vec2& pos, const Vec2& offset)
	{
		if (!_particle) return;

		auto parpos = _particle->getPos();
		_particle->setPos(parpos + offset);
	}

    void SamplePluginParticle::onParticleSelect(const char* particle)
    {
        _particle = particle::ParticleManager::inst().createParticle(particle);
        if (!_particle)
        {
            LogError("create particle ({}) failed.", particle);
            return;
        }

        _particle->start();

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
            _particle->start();
        }
    }
    
    void SamplePluginParticle::onParticleStop()
    {
        if (_particle)
        {
            _particle->stop();
        }
    }
    
    void SamplePluginParticle::onParticleSave()
    {
        if (_particle)
        {
            _particle->save();
        }
    }

    void SamplePluginParticle::onParticleReload()
    {
        if(_particle)
        {
            onParticleSelect(_particle->getName().c_str());
        }
    }
}