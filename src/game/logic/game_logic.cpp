#include "game/logic/game_logic.h"

#include "bevtree/bevtree.h"
#include "utility/translator.h"

#include "game/scene/game_scene.h"
#include "game/scene/object_factory.h"

#include "game/ui/form_loading.h"
#include "game/ui/form_entry.h"
#include "game/ui/form_main.h"
#include "game/ui/form_scenes.h"


#include "game/ui/ui_logic_events.h"



namespace game
{
	GameLogic::GameLogic(GameContext& context):_gameContext(context)
	{
		_scene = std::make_unique<GameScene>(_gameContext);
        _scene->on_load_progress.connect(this, &GameLogic::onSceneLoadProgress);

		_gameContext.setCurrentScene(_scene.get());

		_gameContext.setGameConfig(&_gameConfig);

        ui::GuiManager().inst().on_custom_event.connect(this, &GameLogic::onUICustomEvent);
	}

	GameLogic::~GameLogic()
	{
	}

	void GameLogic::loadResource()
	{
        auto gamecfg = _gameContext.resPath() / "game_config.json";
        bool res = _gameConfig.load(gamecfg);
        if (!res) {
            spdlog::error("load game config: {} failed.", gamecfg.string());
            return;
        }

        auto btreePath =_gameContext.resPath() / "data/bevtree/";
        res = bevtree::BevTreeManager::inst().load(btreePath);
        if (!res) {
            spdlog::error("load bevtree config: {} failed.", btreePath.string());
            return;
        }

        auto roleCfgs =_gameContext.resPath() / "data/role/";
        res = ObjectFactory::inst().loadObjects(_gameContext, roleCfgs);
        if (!res) {
            spdlog::error("load role config: {} failed.", roleCfgs.string());
            return;
        }

        auto itemCfgs =_gameContext.resPath() / "data/item/";
        res = ObjectFactory::inst().loadObjects(_gameContext, itemCfgs);
        if (!res) {
            spdlog::error("load item config: {} failed.", itemCfgs.string());
            return;
        }

        auto skilldir =_gameContext.resPath() / "data/skill/";
        res = ObjectFactory::inst().loadSkills(_gameContext, skilldir);
        if (!res) {
            spdlog::error("load skill config: {} failed.", skilldir.string());
            return;
        }

        auto buffdir =_gameContext.resPath() / "data/buff/";
        res = ObjectFactory::inst().loadBuffs(_gameContext, buffdir);
        if (!res) {
            spdlog::error("load buff config: {} failed.", buffdir.string());
            return;
        }

        auto particleCfgs =_gameContext.resPath() / "particles/";
        res = particle::ParticleManager::inst().LoadParticles(particleCfgs);
        if (!res) {
            spdlog::error("load partiles config: {} failed.", particleCfgs.string());
            return;
        }

        auto textdir =_gameContext.resPath() / "localized/CHS/";
        res = utility::StringTranslator::inst().load(utility::Language::SimpleChinese, textdir);
        if (!res) {
            spdlog::error("load translator file ({}) failed.", textdir.string());
            return;
        }
	}

    void GameLogic::init()
    {
        loadResource();
    }
    
    void GameLogic::shut()
    {
    }

    void GameLogic::update(float delta)
    {
        _scene->onUpdate(delta);
    }

    void GameLogic::draw()
    {
        _scene->onDraw();
    }

    void GameLogic::start()
    {
        auto formEntry = ui::GuiManager::inst().showForm<FormEntry>("form_entry", _gameContext);
        formEntry->on_start_game.connect(this, &GameLogic::onStartNewGame);
        formEntry->on_resume_game.connect(this, &GameLogic::onResumeGame);
        formEntry->on_config_game.connect(this, &GameLogic::onConfigGame);
        formEntry->on_exit_game.connect(this, &GameLogic::onExitGame);
    }

    void GameLogic::pause()
    {
    }

    void GameLogic::onStartNewGame()
    {
        ui::GuiManager::inst().closeForm("form_entry");

        startFirstScene();
    }

    void GameLogic::onResumeGame()
    {
        ui::GuiManager::inst().closeForm("form_entry");
    }

    void GameLogic::onConfigGame()
    {
        //ui::GuiManager::inst().closeForm("form_entry");
    }

    void GameLogic::onExitGame()
    {
        //ui::GuiManager::inst().closeForm("form_entry");
    }

    void GameLogic::startFirstScene()
    {
        showLoadingForm(true);

        auto mapFile = _gameContext.resPath() / "scenes/test/level_test/test_map.tmj";
        auto res = _scene->load(mapFile);
        if (!res) {
            spdlog::error("load level test: {} failed.", mapFile.string());
            return;
        }

        _scene->onStart();
    }

    bool GameLogic::switchScene(const std::string& sceneName)
    {
        showMainForm(false);

        showLoadingForm(true);

        _scene->unload();

        auto mapFile = _gameContext.resPath() / sceneName;
        _scene->load(mapFile);

        return true;
    }

    void GameLogic::showMainForm(bool visible)
    {
        if (visible)
        {
            ui::GuiManager::inst().showForm<FormMain>("form_main", _gameContext);
        }
        else
        {

            ui::GuiManager::inst().closeForm("form_main");
        }
    }

    void GameLogic::showLoadingForm(bool visible)
    {
        if (visible)
        {
            ui::GuiManager::inst().showForm<FormLoading>("form_loging", _gameContext);
        }
        else
        {

            ui::GuiManager::inst().closeForm("form_loging");
        }
    }

    void GameLogic::onSceneLoadProgress(float progress)
    {
        if (progress >= 1.0f)
        {
            showLoadingForm(false);

            showMainForm(true);
        }
        else
        {
            auto pform = ui::GuiManager::inst().getForm<FormLoading>("form_loading");
            if (pform)
            {
                pform->setProgress(progress);
            }
        }
    }

    void GameLogic::onUICustomEvent(int eventId, const utility::VarList& varlist)
    {
        if (eventId == Event_SelectScene)
        {
            int index = varlist[0];
            std::string name = varlist[1];

            switchScene(name);
        }
    }


}