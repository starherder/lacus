#include "game/logic/game_logic.h"

#include "bevtree/bevtree.h"
#include "utility/translator.h"

#include "game/scene/game_scene.h"
#include "game/scene/object_factory.h"

#include "game/ecs/system_motion.h"
#include "game/ecs/system_render.h"
#include "game/ecs/system_bevtree.h"
#include "game/ecs/system_skill.h"
#include "game/ecs/system_pickup.h"
#include "game/ecs/system_dead.h"
#include "game/ecs/system_selection.h"
#include "game/ecs/system_fight.h"
#include "game/ecs/system_buff.h"
#include "game/ecs/system_spawner.h"
#include "game/ecs/system_numerical.h"
#include "game/ecs/system_float_text.h"

#include "game/ui/form_loading.h"
#include "game/ui/form_start.h"
#include "game/ui/form_main.h"
#include "game/ui/form_scenes.h"
#include "game/ui/ui_logic_events.h"
#include "game/ui/form_main.h"
#include "game/ui/imform_debug.h"
#include "game/ui/form_result_fail.h"
#include "game/ui/form_result_success.h"


namespace game
{
	GameLogic::GameLogic(GameContext& context):_gameContext(context)
	{
		_scene = std::make_unique<GameScene>(_gameContext);
        _scene->on_load_progress.connect(this, &GameLogic::onSceneLoadProgress);

		_gameContext.setScene(_scene.get());
		_gameContext.setGameConfig(&_gameConfig);

        ui::GuiManager().inst().on_custom_event.connect(this, &GameLogic::onUICustomEvent);

        initEscSystem();
	}

	GameLogic::~GameLogic()
	{
	}

    void GameLogic::initEscSystem()
    {
        EcsSystemManager::inst().init(_gameContext);
    }
    
	void GameLogic::loadResource()
	{
        ObjectFactory::inst().init(&_gameContext);

        auto gamecfg = _gameContext.resPath() / "game_config.json";
        bool res = _gameConfig.load(gamecfg);
        if (!res) {
            SPDLOG_ERROR("load game config: {} failed.", gamecfg.string());
        }

        auto btreePath =_gameContext.resPath() / "data/bevtree/";
        res = bevtree::BevTreeManager::inst().load(btreePath);
        if (!res) {
            SPDLOG_ERROR("load bevtree config: {} failed.", btreePath.string());
        }

        auto roleCfgs =_gameContext.resPath() / "data/role/";
        res = ObjectFactory::inst().loadRoles(roleCfgs);
        if (!res) {
            SPDLOG_ERROR("load role config: {} failed.", roleCfgs.string());
        }

        auto enemyCfgs = _gameContext.resPath() / "data/role/";
        res = ObjectFactory::inst().loadEnemies(enemyCfgs);
        if (!res) {
            SPDLOG_ERROR("load enemy config: {} failed.", enemyCfgs.string());
        }

        auto otherCfgs = _gameContext.resPath() / "data/other/";
        res = ObjectFactory::inst().loadOther(otherCfgs);
        if (!res) {
            SPDLOG_ERROR("load other config: {} failed.", otherCfgs.string());
        }

        auto itemCfgs =_gameContext.resPath() / "data/item/";
        res = ObjectFactory::inst().loadItems(itemCfgs);
        if (!res) {
            SPDLOG_ERROR("load item config: {} failed.", itemCfgs.string());
        }

        auto skilldir =_gameContext.resPath() / "data/skill/";
        res = ObjectFactory::inst().loadSkills(skilldir);
        if (!res) {
            SPDLOG_ERROR("load skill config: {} failed.", skilldir.string());
        }

        auto buffdir =_gameContext.resPath() / "data/buff/";
        res = ObjectFactory::inst().loadBuffs(buffdir);
        if (!res) {
            SPDLOG_ERROR("load buff config: {} failed.", buffdir.string());
        }

        auto particleCfgs =_gameContext.resPath() / "particles/";
        res = particle::ParticleManager::inst().LoadParticles(particleCfgs);
        if (!res) {
            SPDLOG_ERROR("load partiles config: {} failed.", particleCfgs.string());
        }

        auto textdir =_gameContext.resPath() / "localized/CHS/";
        res = utility::StringTranslator::inst().load(utility::Language::SimpleChinese, textdir);
        if (!res) {
            SPDLOG_ERROR("load translator file ({}) failed.", textdir.string());
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
        if (!_scene->ready())
        {
            return;
        }

        _scene->onUpdate(delta);

        checkGameState();

        EcsSystemManager::inst().update(delta);
    }

    void GameLogic::draw()
    {
        if (!_scene->ready())
        {
            return;
        }

        _scene->onDraw();

        EcsSystemManager::inst().draw();
    }

    void GameLogic::start()
    {
        auto formStart = ui::GuiManager::inst().createForm<FormStart>("form_start", _gameContext);
        formStart->on_start_game.connect(this, &GameLogic::onStartNewGame);
        formStart->on_resume_game.connect(this, &GameLogic::onResumeGame);
        formStart->on_config_game.connect(this, &GameLogic::onConfigGame);
        formStart->on_exit_game.connect(this, &GameLogic::onExitGame);

        auto form_debug = imgui::ImFormManager::inst().showForm<ImFormDebug>("ImFormDebug");
        if (form_debug)
        {
            form_debug->init(&_gameContext);
            form_debug->on_reload_res.connect(this, &GameLogic::onDebugReloadResource);
        }
    }

    void GameLogic::checkGameState()
    {
        if (_state != GameState::Running)
        {
            return;
        }

        int liveRole = 0;
        auto views = _gameContext.registry().view<CompComm>();
        for (auto& ent : views)
        {
            auto pdead = _gameContext.registry().try_get<CompDead>(ent);
            if (pdead) {
                continue;
            }

            auto& comp = views.get<CompComm>(ent);
            if (comp.side == CampSide::Gangster) {
                liveRole++;
            }
        }

        int handCards = (int)_gameContext.scene().dataCenter().getHandCards().size();

        if (liveRole == 0 && handCards == 0)
        {
            _state = GameState::Finish;
            ui::GuiManager::inst().createForm<FormResultFail>("form_fail", _gameContext);
        }
    }

    void GameLogic::pause()
    {
        imgui::ImFormManager::inst().closeForm("ImFormDebug");
    }

    void GameLogic::onStartNewGame()
    {
        startFirstScene();
    }

    void GameLogic::onResumeGame()
    {
    }

    void GameLogic::onConfigGame()
    {
    }

    void GameLogic::onExitGame()
    {
    }

    void GameLogic::startFirstScene()
    {
        closeAllForms();

        showLoadingForm(true);

        ui::GuiManager::inst().createForm<FormScenes>("form_scenes", _gameContext);

#if 0
        auto mapFile = _gameContext.resPath() / _gameConfig.scenes.first_scene;
        auto res = _scene->load(mapFile);
        if (!res) {
            SPDLOG_ERROR("load level test: {} failed.", mapFile.string());
            return;
        }
#endif
        _scene->onStart();
    }

    bool GameLogic::switchScene(const std::string& sceneName)
    {
        closeAllForms();

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
            ui::GuiManager::inst().createForm<FormMain>("form_main", _gameContext);
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
            ui::GuiManager::inst().createForm<FormLoading>("form_loading", _gameContext);
        }
        else
        {
            ui::GuiManager::inst().closeForm("form_loading");
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
            switchScene(varlist[0]);
        }
        else if (eventId == Event_ToggleMainForm)
        {
            showMainForm(varlist[0]);
        }
    }

    void GameLogic::onDebugReloadResource()
    {
        ObjectFactory::inst().reloadAll();
    }

    void GameLogic::closeAllForms()
    {
        ui::GuiManager::inst().closeAllForms();
    }
}