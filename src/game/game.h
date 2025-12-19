#pragma once

#include "engine/application.h"

#include "scene/game_scene.h"

namespace game {

    class GameLogicPlugin: public engine::Plugin {

    public:
        GameLogicPlugin() = delete;
        GameLogicPlugin(engine::Application& app);
        ~GameLogicPlugin() = default
        
        const char* name() override {return "game_logic_plugin"; } 

        void onInit() override;
        void onUpdate() override;
        void onDraw() override;
        void onDrawUI() override;
        void onClose() override;
        void onEvent(const Event& event)  override;

    private:
        GameScene _scene;
        engine::Application& _app;
    };


    class Game {
    public:
        Game();
        ~Game();

        bool init(engine::Application& app);
        
    };
}