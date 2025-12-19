#pragma once

#include "application.h"
namespace engine 
{
    class Scene {
    public:
        Scene() = delete;
        Scene(Scene&&) = delete;
        Scene(const Scene&) = delete;

        Scene(Application& app) : _app(app) {}
        virtual ~Scene() = default;

        virtual bool load(const std::string& scenePath) = 0;
        virtual bool unload() = 0;

        virtual void onUpdate(float deltaTime) = 0;
        virtual void onRender() = 0;
        virtual void onRenderUI() = 0;

    private:
        Application& _app;
    };

}