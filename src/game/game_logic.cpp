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
    auto form = ui::GuiManager::inst().showForm<ui::FormDemo>("main_frame");
    //form->hide();
}

void GameLogicPlugin::onUpdate()
{
    auto delta = _app.fpsChecker().deltaSeconds();
    _scene->onUpdate(delta);

    ui::GuiManager::inst().update(delta);
}

void GameLogicPlugin::onDraw() 
{
    _scene->onDraw();
}

void GameLogicPlugin::onDrawUI()
{
    ui::GuiManager::inst().draw();
}

void GameLogicPlugin::onClose()
{
    _scene->unload();
}

}