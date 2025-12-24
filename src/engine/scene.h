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

        void update();
        void draw();
        void drawUI();

        virtual bool load(const engine::fs::path& filePath) = 0;
        virtual bool unload() = 0;

        virtual void onStart() = 0;
        virtual void onStop() = 0;

        virtual void onUpdate(float deltaTime) = 0;
        virtual void onDraw() = 0;

        auto& application() { return _app; }
        
    private:
        Application& _app;
    };

}