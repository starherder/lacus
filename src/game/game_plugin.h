#pragma once

#include "engine/application.h"

#include "scene/game_scene.h"
#include "game/game_config.h"
#include "game/logic/game_logic.h"

namespace game {

class GamePlugin: public engine::Plugin 
{
public:
    GamePlugin() = delete;
    GamePlugin(GamePlugin&&) = delete;
    GamePlugin(const GamePlugin&) = delete;

    GamePlugin(engine::Application& app);
    ~GamePlugin() = default;
        
    const char* name() override {return "game_plugin"; } 

    void onInit() override;
    void onUpdate() override;
    void onDraw() override;
    void onDrawUI() override;
    void onClose() override;

    void onEnable() override;
    void onDisable() override;

    GameContext& context() { return _gameContext; }

private:
    std::unique_ptr<GameLogic> _gameLogic = nullptr;

    GameContext _gameContext;

    engine::Application& _app;
};
}