#pragma once

#include "engine/application.h"

#include "scene/game_scene.h"


namespace game {

class GameLogicPlugin: public engine::Plugin 
{
public:
    GameLogicPlugin() = delete;
    GameLogicPlugin(engine::Application& app);
    ~GameLogicPlugin() = default;
        
    const char* name() override {return "game_logic_plugin"; } 

    void onInit() override;
    void onUpdate() override;
    void onDraw() override;
    void onDrawUI() override;
    void onClose() override;

    void onEnable() override;
    void onDisable() override;

private:
    std::unique_ptr<GameScene> _scene = nullptr;

    engine::Application& _app;
};
}