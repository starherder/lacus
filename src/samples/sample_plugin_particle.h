#pragma once

#include "engine/application.h"
#include "imform/imform_manager.h"

namespace particle {
    class Particle;
}

namespace samples {

    using namespace engine;

    class ImFormParticles : public imgui::ImForm
    {
    public:
        ImFormParticles();
        ~ImFormParticles() = default;

        signals::Signal<const char*> on_select_particle;
        signals::Signal<> on_particle_start;
        signals::Signal<> on_particle_stop;

    private:
        void draw() override;

        void init();

    private:
        int _select_index = 0;
        std::vector<const char*> _particles;
    };

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

    private:
        std::shared_ptr<particle::Particle> _particle = nullptr;
    };
}