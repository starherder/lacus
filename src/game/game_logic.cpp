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

    auto btreePath = application()->resPath() / "data/bevtree/";
    bool res = bevtree::BevTreeManager::inst().load(btreePath);
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