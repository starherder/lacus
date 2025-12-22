#include "game_logic.h"

#include "ui/gui_manager.h"
#include "ui/form_demo.h"

namespace game {


GameLogicPlugin::GameLogicPlugin(engine::Application& app) 
    : _app(app)
{
    _scene = std::make_unique<GameScene>(app);
}

void GameLogicPlugin::onInit() 
{
    _scene->load(_app.resPath()/"scenes/Medieval/sea.tmj");

    ui::GuiManager::inst().init(&_app);
    ui::GuiManager::inst().showForm<ui::FormDemo>("main_frame");
}

void GameLogicPlugin::onUpdate()
{
    auto delta = _app.fpsChecker().deltaSeconds();
    _scene->onUpdate(delta);

    ui::GuiManager::inst().update(delta);
}

void GameLogicPlugin::onDraw() 
{
    _scene->onRender();
}

void GameLogicPlugin::onDrawUI()
{
    _scene->onRenderUI();

    ui::GuiManager::inst().draw();
}

void GameLogicPlugin::onClose()
{
    _scene->unload();
}

}