#include "game.h"

namespace game {


GameLogicPlugin::GameLogicPlugin(engine::Application& app) 
    : _app(app)
{
}

void GameLogicPlugin::onInit() 
{
    _scene.load("scenes/Medieval/sea.tmj");
}

void GameLogicPlugin::onUpdate()
{
    _scene.onUpdate();
}

void GameLogicPlugin::onDraw() 
{
    _scene.onRender();
}

void GameLogicPlugin::onDrawUI()
{
    _scene.onRenderUI();
}

void GameLogicPlugin::onClose()
{
    _scene.unload();
}

void GameLogicPlugin::onEvent(const Event& event)
{
}


///////////////////////////////////////////////////////////////////////////

bool Game::init(engine::Application& app)
{
    spdlog::info("Game::init");

    auto plugin = app->addPlugin<GameLogicPlugin>(engine::PluginPriority::Bottom, app);
    return plugin != nullptr;
}


}