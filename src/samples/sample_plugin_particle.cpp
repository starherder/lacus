#include "sample_plugin_particle.h"

#include "engine/application.h"
#include "particle/particle_manager.h"

#include <algorithm>

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

	struct EnumItem
	{
		const char* name;
		int value;
	};

	static const char* getEnumName(const EnumItem* items, int count, int value)
	{
		for (int i = 0; i < count; ++i)
		{
			if (items[i].value == value)
			{
				return items[i].name;
			}
		}
		return "Unknown";
	}

	static bool enumCombo(const char* label, int& value, const EnumItem* items, int count)
	{
		bool modified = false;
		if (ImGui::BeginCombo(label, getEnumName(items, count, value)))
		{
			for (int i = 0; i < count; ++i)
			{
				bool selected = value == items[i].value;
				if (ImGui::Selectable(items[i].name, selected))
				{
					value = items[i].value;
					modified = true;
				}
				if (selected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}
		return modified;
	}

	static Vec2 editorPreviewParticlePos()
	{
		ImVec2 size = ImGui::GetIO().DisplaySize;
		if (size.x <= 0.0f || size.y <= 0.0f)
		{
			return {480.0f, 360.0f};
		}

		return {size.x * 0.5f - std::min(240.0f, size.x * 0.18f), size.y * 0.5f};
	}

	static const EnumItem CoordinateSpaceItems[] = {
		{"Inherit", (int)CoordinateSpace::Inherit},
		{"World", (int)CoordinateSpace::World},
		{"Screen", (int)CoordinateSpace::Screen}
	};

	static const EnumItem BlendModeItems[] = {
		{"None", (int)BlendMode::None},
		{"Blend", (int)BlendMode::Blend},
		{"Blend premultiplied", (int)BlendMode::BlendPremultiplied},
		{"Add", (int)BlendMode::Add},
		{"Add premultiplied", (int)BlendMode::AddPremultiplied},
		{"Mod", (int)BlendMode::Mod},
		{"Mul", (int)BlendMode::Mul}
	};

	static const EnumItem EmitterShapeItems[] = {
		{"Point", (int)EmitterShape::Point},
		{"Line", (int)EmitterShape::Line},
		{"Rect", (int)EmitterShape::Rect},
		{"Circle", (int)EmitterShape::Circle}
	};

	static const EnumItem MotionModeItems[] = {
		{"Free", (int)MotionMode::Free},
		{"Relative", (int)MotionMode::Relative}
	};

	static const EnumItem EmitterTypeItems[] = {
		{"Gravity", (int)EmitterType::Gravity},
		{"Radius", (int)EmitterType::Radius}
	};

	static const EnumItem ValueCurveItems[] = {
		{"Linear", (int)ValueCurve::Linear},
		{"EaseIn", (int)ValueCurve::EaseIn},
		{"EaseOut", (int)ValueCurve::EaseOut},
		{"EaseInOut", (int)ValueCurve::EaseInOut},
		{"SmoothStep", (int)ValueCurve::SmoothStep},
		{"Pulse", (int)ValueCurve::Pulse}
	};

	static const EnumItem TrailModeItems[] = {
		{"None", (int)TrailMode::None},
		{"Stretch", (int)TrailMode::Stretch}
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

			ImGui::Checkbox("debug emitter", &_showEmitterDebug);

			char bufTexture[128] = { 0 };
			strcpy_s(bufTexture, sizeof(bufTexture), _particleDescript.texture.c_str());
			if (ImGui::InputText("texture", bufTexture, sizeof(bufTexture)))
			{
				_particleDescript.texture = bufTexture;
				_modified = true;
			}

			int coordinateSpace = (int)_particleDescript.coordinateSpace;
			if (enumCombo("coordinate space", coordinateSpace, CoordinateSpaceItems, IM_ARRAYSIZE(CoordinateSpaceItems)))
			{
				_particleDescript.coordinateSpace = (CoordinateSpace)coordinateSpace;
				_modified = true;
			}

			int blendMode = (int)_particleDescript.blendMode;
			if (enumCombo("blend mode", blendMode, BlendModeItems, IM_ARRAYSIZE(BlendModeItems)))
			{
				_particleDescript.blendMode = (BlendMode)blendMode;
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
			if (enumCombo("emitter shape", emitShape, EmitterShapeItems, IM_ARRAYSIZE(EmitterShapeItems)))
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
				_particleDescript.emitRate = 0.0f;
				_modified = true;
			}

			if (ImGui::InputFloat("emission rate", &_particleDescript.emitRate))
			{
				_modified = true;
			}

			if (ImGui::InputFloat("duration", &_particleDescript.duration))
			{
				_modified = true;
			}

			int motionMode = (int)_particleDescript.motionMode;
			if (enumCombo("motion mode", motionMode, MotionModeItems, IM_ARRAYSIZE(MotionModeItems)))
			{
				_particleDescript.motionMode = (MotionMode)motionMode;
				_modified = true;
			}

			int emitType = (int)_particleDescript.emitterType;
			if (enumCombo("emitter type", emitType, EmitterTypeItems, IM_ARRAYSIZE(EmitterTypeItems)))
			{
				_particleDescript.emitterType = (EmitterType)emitType;
				_modified = true;
			}

			float wind[2] = { _particleDescript.windMode.force.x, _particleDescript.windMode.force.y };
			if (ImGui::InputFloat2("wind", wind))
			{
				_particleDescript.windMode.force = { wind[0], wind[1] };
				_modified = true;
			}

			float windVar[2] = { _particleDescript.windMode.forceVar.x, _particleDescript.windMode.forceVar.y };
			if (ImGui::InputFloat2("wind var", windVar))
			{
				_particleDescript.windMode.forceVar = { windVar[0], windVar[1] };
				_modified = true;
			}

			float windWave[2] = { _particleDescript.windMode.wave.x, _particleDescript.windMode.wave.y };
			if (ImGui::InputFloat2("wind wave", windWave))
			{
				_particleDescript.windMode.wave = { windWave[0], windWave[1] };
				_modified = true;
			}

			if (ImGui::InputFloat("wind wave speed", &_particleDescript.windMode.waveSpeed))
			{
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
				_particleDescript.emitRate = 0.0f;
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

			int colorCurve = (int)_particleDescript.colorCurve;
			if (enumCombo("color curve", colorCurve, ValueCurveItems, IM_ARRAYSIZE(ValueCurveItems)))
			{
				_particleDescript.colorCurve = (ValueCurve)colorCurve;
				_modified = true;
			}

			int sizeCurve = (int)_particleDescript.sizeCurve;
			if (enumCombo("size curve", sizeCurve, ValueCurveItems, IM_ARRAYSIZE(ValueCurveItems)))
			{
				_particleDescript.sizeCurve = (ValueCurve)sizeCurve;
				_modified = true;
			}

			int spinCurve = (int)_particleDescript.spinCurve;
			if (enumCombo("spin curve", spinCurve, ValueCurveItems, IM_ARRAYSIZE(ValueCurveItems)))
			{
				_particleDescript.spinCurve = (ValueCurve)spinCurve;
				_modified = true;
			}

			int trailMode = (int)_particleDescript.trailMode;
			if (enumCombo("trail mode", trailMode, TrailModeItems, IM_ARRAYSIZE(TrailModeItems)))
			{
				_particleDescript.trailMode = (TrailMode)trailMode;
				_modified = true;
			}

			float trailParams[3] = {
				_particleDescript.trailLength,
				_particleDescript.trailWidth,
				_particleDescript.trailAlphaScale
			};
			if (ImGui::InputFloat3("trail len/width/alpha", trailParams))
			{
				_particleDescript.trailLength = trailParams[0];
				_particleDescript.trailWidth = trailParams[1];
				_particleDescript.trailAlphaScale = trailParams[2];
				_modified = true;
			}

			char subEmitterBuff[128] = { 0 };
			strcpy_s(subEmitterBuff, sizeof(subEmitterBuff), _particleDescript.subEmitter.c_str());
			if (ImGui::InputText("sub emitter", subEmitterBuff, sizeof(subEmitterBuff)))
			{
				_particleDescript.subEmitter = subEmitterBuff;
				_modified = true;
			}

			if (ImGui::InputInt("sub emitter count", &_particleDescript.subEmitterCount))
			{
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
				auto previewPos = _particle->getPos();
                _particle->correctDescription(_particleDescript);
                _particle->setDescription(_particleDescript);
				_particle->setPos(previewPos);
                
				_modified = false;
			}
		}

		ImGui::End();
	}

    /////////////////////////////////////////////////////////////////////////////////////////////

    void SamplePluginParticle::onInit() 
    {
        ParticleManager::inst().init(application());

		auto texturedir = application()->resPath() / "textures/";
		application()->resourceManager().textureManager().loadAllTexSets(texturedir);

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
            _particle->draw(application()->painter());
			auto form = imgui::ImFormManager::inst().getForm<ImParticleEditor>("particle_editor");
			auto emitter = _particle->getEmitter();
			if (form && form->showEmitterDebug() && emitter)
			{
				emitter->drawDebug(application()->painter(), Color::Cyan, 2.0f);
			}
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
		_particle->setPos(editorPreviewParticlePos());

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
