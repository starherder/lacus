#include "game_logic.h"

#include "ui/gui_manager.h"
#include "ui/form_demo.h"

#include "bevtree/bevtree.h"
#include "utility/translator.h"

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

    auto btreePath = application()->resPath() / "roles/bevtree/bev_common_npc.xml";
    bool res = bevtree::BevTreeManager::inst().load(btreePath);
    if (!res) {
        spdlog::error("load bevtree config: {} failed.", btreePath.string());
        return;
    }

    auto roleCfgs = application()->resPath() / "roles/roles.json";
    res = RoleFactory::inst().load(_gameContext, roleCfgs);
    if (!res) {
        spdlog::error("load role config: {} failed.", roleCfgs.string());
        return;
    }

    auto textFile = application()->resPath() / "localized/CHS/npc.txt";
    res = utility::StringTranslator::inst().load(utility::Language::SimpleChinese, textFile);
    if (!res) {
        spdlog::error("load translator file ({}) failed.", textFile.string());
        return;
    }

    res = _scene->load(_app.resPath()/"scenes/level_test/test_map.tmj");
    if (!res) {
        spdlog::error("load role config: {} failed.", roleCfgs.string());
        return;
    }

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