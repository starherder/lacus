#pragma once

#include "engine/application.h"
#include "sample_camera.h"

#include "entt/entt.hpp"
#include "imform/imform.h"

namespace engine {
    class Texture;
}

namespace samples {

    class ImFormEntt : public imgui::ImForm
    {
    public:
        ImFormEntt() = delete;
        ImFormEntt(engine::Application* app, class SamplePluginEntt* plugin)
            : _application(app), _plugin(plugin) {}
        ~ImFormEntt() = default;

    protected:
        void onInit() override;

        void draw() override;

    private:
        engine::Application* _application = nullptr;
        class SamplePluginEntt* _plugin = nullptr;
    };


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

        void enableEntt(bool enable);

        auto& registry() { return _registry; }

    private:
        void initEntities();

        void onEntityDrawSystem();

    private:
        entt::registry _registry;
        entt::dispatcher _dispatcher;

        bool _enttEnable = false;

        std::unique_ptr<engine::Camera> _camera = nullptr;
    };
}