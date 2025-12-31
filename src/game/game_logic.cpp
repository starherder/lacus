#include "game_logic.h"

#include "ui/gui_manager.h"
#include "ui/form_demo.h"

#include "bevtree/bevtree.h"

namespace game {


GameLogicPlugin::GameLogicPlugin(engine::Application& app) 
    : _app(app), _gameContext(app)
{
    _scene = std::make_unique<GameScene>(_gameContext);
    _gameContext.setCurrentScene(_scene.get());
}

void GameLogicPlugin::onInit() 
{
    ui::GuiManager::inst().init(&_app);

    auto filepath = application()->resPath() / "npc/bevtree/bev_common_npc.xml";
    bevtree::BevTreeManager::inst().load(filepath);

    _scene->load(_app.resPath()/"scenes/level_test/test_map.tmj");

    //ui::GuiManager::inst().showForm<ui::FormDemo>("form_demo");
}

void GameLogicPlugin::onUpdate()
{
    auto delta = _app.frameTicker().deltaSeconds();
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

void GameLogicPlugin::onEnable()
{
    _scene->onStart();
}

void GameLogicPlugin::onDisable()
{
    _scene->onStop();
}

}