#pragma once

#include "application.h"
namespace engine 
{
    enum class SceneState {
        None,
        Loading,
        Unloading,
        Ready,
    };


    class Scene 
    {
    public:
        signals::Signal<float> on_load_progress;

    public:
        Scene() = delete;
        Scene(Scene&&) = delete;
        Scene(const Scene&) = delete;

        Scene(Application& app) : _app(app) {}
        virtual ~Scene() = default;

        void update();
        void draw();

        bool ready() { return _state == (int)SceneState::Ready; }
        
        virtual bool load(const engine::fs::path& filePath) = 0;
        virtual bool unload() = 0;

        virtual void onStart() = 0;
        virtual void onStop() = 0;

        virtual void onUpdate(float deltaTime) = 0;
        virtual void onDraw() = 0;

        auto& application() { return _app; }
        
    protected:
        Application& _app;

        std::atomic<int> _state = (int)SceneState::None;
    };

}