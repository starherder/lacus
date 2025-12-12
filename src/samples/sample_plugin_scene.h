#pragma once

#include "engine/application.h"
#include "engine/camera.h"

#include "entt/entt.hpp"

namespace samples {

    class SamplePluginScene final : public engine::Plugin 
    {
    public:
        SamplePluginScene() = default;
        ~SamplePluginScene() = default;

        const char* name() override;

        void onInit() override;

        void onInstall() override;

        void onUninstall() override;

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
    };
}