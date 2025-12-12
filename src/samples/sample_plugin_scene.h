#pragma once

#include "engine/application.h"

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
    
    private:
        void initEntities();

        void onEntityDrawSystem();

    private:
        entt::registry _registry;
        entt::dispatcher _dispatcher;

        static const int _xcount = 200;
        static const int _ycount = 100;
        
        static const int _gridw = 40;
        static const int _gridh = 40;
    };
}