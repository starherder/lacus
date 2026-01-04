#pragma once

#include "engine/application.h"
#include "imform/imform_manager.h"
#include "particle/particle_manager.h"

namespace samples 
{
    using namespace engine;
    using namespace particle;
    

	class ImParticleEditor : public imgui::ImForm
	{
    public:
        signals::Signal<const char*> on_particle_select;
        signals::Signal<> on_particle_start;
        signals::Signal<> on_particle_stop;
        signals::Signal<> on_particle_reload;
        signals::Signal<> on_particle_save;

	public:
		ImParticleEditor(Particle* particle);
		~ImParticleEditor();

		void init();
		void draw() override;

        void setParticle(Particle* particle) { _particle = particle; }

	private:
		int _selectIndex = 0;
		std::vector<const char*> _nameVector;

		bool _modified = false;
        Particle* _particle = nullptr;
	};

    ///////////////////////////////////////////////////////////////////////////////////////////////////

    class SamplePluginParticle final : public engine::Plugin , public signals::SlotHandler
    {
    public:
        SamplePluginParticle() = default;

        ~SamplePluginParticle() = default;

        const char* name() override { return "sample_particle_plugin"; }

        void onInit() override ;

        void onInstall() override ;

        void onUninstall() override ;

        void onEnable() override;

        void onDisable() override;

        void onUpdate() override ;

        void onDraw() override ;

        void onClose() override ;

    private:
        void onParticleSelect(const char* particle);

        void onParticleStart();

        void onParticleStop();

        void onParticleSave();

        void onParticleReload();

    private:
        std::shared_ptr<Particle> _particle = nullptr;
    };
}