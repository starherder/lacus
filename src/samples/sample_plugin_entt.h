#pragma once

#include "engine/application.h"
#include "sample_camera.h"

#include "entt/entt.hpp"

namespace engine {
    class Texture;
}

namespace samples {

    class SamplePluginEntt final : public engine::Plugin 
    {
    public:
        SamplePluginEntt() = default;
        ~SamplePluginEntt() = default;

        const char* name() override { return "sample_entt_plugin";}

        void onInit() override;

        void onInstall() override;

        void onUninstall() override;

        void onEnable() override;

        void onDisable() override;

        void onUpdate() override;

        void onDraw() override;

        void onClose() override;

        void onEvent(const engine::Event& event) override;
    
    private:
        void initEntities();

        void onEntityDrawSystem();

    private:
        entt::registry _registry;
        entt::dispatcher _dispatcher;

        static const int _xcount = 100;
        static const int _ycount = 100;
        
        static const int _gridw = 50;
        static const int _gridh = 50;

        std::unique_ptr<engine::Camera> _camera = nullptr;

        engine::Texture* _texture = nullptr;
    };
}