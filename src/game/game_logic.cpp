#include "game_logic.h"

#include "ui/gui_manager.h"

#include "bevtree/bevtree.h"
#include "utility/translator.h"

#include "samples/sample_plugin_ui.h"

namespace game {


GameLogicPlugin::GameLogicPlugin(engine::Application& app) 
    : _app(app), _gameContext(app)
{
    _scene = std::make_unique<GameScene>(_gameContext);
    
    _gameContext.setCurrentScene(_scene.get());

    _gameContext.setGameConfig(&_gameConfig);
}

void GameLogicPlugin::onInit() 
{
    ui::GuiManager::inst().init(&_app);

    auto gamecfg = application()->resPath() / "game_config.json";
    bool res = _gameConfig.load(gamecfg);
    if (!res) {
        spdlog::error("load game config: {} failed.", gamecfg.string());
        return;
    }

    auto btreePath = application()->resPath() / "data/bevtree/";
    res = bevtree::BevTreeManager::inst().load(btreePath);
    if (!res) {
        spdlog::error("load bevtree config: {} failed.", btreePath.string());
        return;
    }

    auto roleCfgs = application()->resPath() / "data/role/";
    res = ObjectFactory::inst().loadObjects(_gameContext, roleCfgs);
    if (!res) {
        spdlog::error("load role config: {} failed.", roleCfgs.string());
        return;
    }

    auto itemCfgs = application()->resPath() / "data/item/";
    res = ObjectFactory::inst().loadObjects(_gameContext, itemCfgs);
    if (!res) {
        spdlog::error("load item config: {} failed.", itemCfgs.string());
        return;
    }

    auto skilldir = application()->resPath() / "data/skill/";
    res = ObjectFactory::inst().loadSkills(_gameContext, skilldir);
    if (!res) {
        spdlog::error("load skill config: {} failed.", skilldir.string());
        return;
    }

    auto buffdir = application()->resPath() / "data/buff/";
    res = ObjectFactory::inst().loadBuffs(_gameContext, buffdir);
    if (!res) {
        spdlog::error("load buff config: {} failed.", buffdir.string());
        return;
    }

    auto particleCfgs = application()->resPath() / "particles/";
    res = particle::ParticleManager::inst().LoadParticles(particleCfgs);
    if (!res) {
        spdlog::error("load partiles config: {} failed.", particleCfgs.string());
        return;
    }

    auto textdir = application()->resPath() / "localized/CHS/";
    res = utility::StringTranslator::inst().load(utility::Language::SimpleChinese, textdir);
    if (!res) {
        spdlog::error("load translator file ({}) failed.", textdir.string());
        return;
    }

    auto mapFile = _app.resPath() /"scenes/level_test/test_map.tmj";
    res = _scene->load(mapFile);
    if (!res) {
        spdlog::error("load level test: {} failed.", mapFile.string());
        return;
    }

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

    ui::GuiManager::inst().showForm<samples::FormDemo>("form_demo");
}

void GameLogicPlugin::onDisable()
{
    ui::GuiManager::inst().closeForm("form_demo");
    _scene->onStop();
}

}