#include "game_plugin.h"
#include "ui/gui_manager.h"

namespace game 
{

GamePlugin::GamePlugin(engine::Application& app) 
    : _app(app), _context(app)
{
}

void GamePlugin::onInit() 
{
    ui::GuiManager::inst().init(&_app);

    _gameLogic = std::make_unique<GameLogic>(_context);
    _gameLogic->init();
}

void GamePlugin::onUpdate()
{
    auto delta = _app.frameTicker().deltaSeconds();

    _gameLogic->update(delta);

    ui::GuiManager::inst().update(delta);
}

void GamePlugin::onDraw() 
{
    _gameLogic->draw();
}

void GamePlugin::onDrawUI()
{
    ui::GuiManager::inst().draw();
}

void GamePlugin::onClose()
{
    _gameLogic->shut();
}

void GamePlugin::onEnable()
{
    _gameLogic->start();
}

void GamePlugin::onDisable()
{
    _gameLogic->pause();
}

}