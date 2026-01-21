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


namespace game
{
	GameLogic::GameLogic(GameContext& context):_gameContext(context)
	{
		_scene = std::make_unique<GameScene>(_gameContext);
        _scene->on_load_progress.connect(this, &GameLogic::onSceneLoadProgress);

		_gameContext.setCurrentScene(_scene.get());

		_gameContext.setGameConfig(&_gameConfig);

        ui::GuiManager().inst().on_custom_event.connect(this, &GameLogic::onUICustomEvent);

        initEscSystem();
	}

	GameLogic::~GameLogic()
	{
	}

    void GameLogic::initEscSystem()
    {
        _ecsSystems.insert({ EcsPriority::Middle, std::make_shared<MotionSystem>(_gameContext) });
        _ecsSystems.insert({ EcsPriority::Middle, std::make_shared<RenderSystem>(_gameContext) });
        _ecsSystems.insert({ EcsPriority::Middle, std::make_shared<BevTreeSystem>(_gameContext) });
        _ecsSystems.insert({ EcsPriority::Middle, std::make_shared<PickupSystem>(_gameContext) });
        _ecsSystems.insert({ EcsPriority::Middle, std::make_shared<DeadSystem>(_gameContext) });
        _ecsSystems.insert({ EcsPriority::Middle, std::make_shared<SelectionSystem>(_gameContext) });
        _ecsSystems.insert({ EcsPriority::Middle, std::make_shared<SkillSystem>(_gameContext) });
        _ecsSystems.insert({ EcsPriority::Middle, std::make_shared<FightSystem>(_gameContext) });
        _ecsSystems.insert({ EcsPriority::Middle, std::make_shared<BuffSystem>(_gameContext) });
        _ecsSystems.insert({ EcsPriority::Middle, std::make_shared<BuffSystem>(_gameContext) });
        _ecsSystems.insert({ EcsPriority::Middle, std::make_shared<SpawnerSystem>(_gameContext) });
        _ecsSystems.insert({ EcsPriority::Middle, std::make_shared<NumericalSystem>(_gameContext) });
    }
    
	void GameLogic::loadResource()
	{
        auto gamecfg = _gameContext.resPath() / "game_config.json";
        bool res = _gameConfig.load(gamecfg);
        if (!res) {
            SPDLOG_ERROR("load game config: {} failed.", gamecfg.string());
            return;
        }

        auto btreePath =_gameContext.resPath() / "data/bevtree/";
        res = bevtree::BevTreeManager::inst().load(btreePath);
        if (!res) {
            SPDLOG_ERROR("load bevtree config: {} failed.", btreePath.string());
            return;
        }

        auto roleCfgs =_gameContext.resPath() / "data/role/";
        res = ObjectFactory::inst().loadObjects(_gameContext, roleCfgs);
        if (!res) {
            SPDLOG_ERROR("load role config: {} failed.", roleCfgs.string());
            return;
        }

        auto itemCfgs =_gameContext.resPath() / "data/item/";
        res = ObjectFactory::inst().loadObjects(_gameContext, itemCfgs);
        if (!res) {
            SPDLOG_ERROR("load item config: {} failed.", itemCfgs.string());
            return;
        }

        auto skilldir =_gameContext.resPath() / "data/skill/";
        res = ObjectFactory::inst().loadSkills(_gameContext, skilldir);
        if (!res) {
            SPDLOG_ERROR("load skill config: {} failed.", skilldir.string());
            return;
        }

        auto buffdir =_gameContext.resPath() / "data/buff/";
        res = ObjectFactory::inst().loadBuffs(_gameContext, buffdir);
        if (!res) {
            SPDLOG_ERROR("load buff config: {} failed.", buffdir.string());
            return;
        }

        auto particleCfgs =_gameContext.resPath() / "particles/";
        res = particle::ParticleManager::inst().LoadParticles(particleCfgs);
        if (!res) {
            SPDLOG_ERROR("load partiles config: {} failed.", particleCfgs.string());
            return;
        }

        auto textdir =_gameContext.resPath() / "localized/CHS/";
        res = utility::StringTranslator::inst().load(utility::Language::SimpleChinese, textdir);
        if (!res) {
            SPDLOG_ERROR("load translator file ({}) failed.", textdir.string());
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
        if (!_scene->ready())
        {
            return;
        }

        _scene->onUpdate(delta);

        for (auto& [prio, sys] : _ecsSystems)
        {
            sys->update(delta);
        }
    }

    void GameLogic::draw()
    {
        if (!_scene->ready())
        {
            return;
        }

        _scene->onDraw();

        for (auto& [prio, sys] : _ecsSystems)
        {
            sys->draw();
        }
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

        auto mapFile = _gameContext.resPath() / _gameConfig.scenes.first_scene;
        auto res = _scene->load(mapFile);
        if (!res) {
            SPDLOG_ERROR("load level test: {} failed.", mapFile.string());
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
            ui::GuiManager::inst().showForm<FormLoading>("form_loading", _gameContext);
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
            int index = varlist[0];
            std::string name = varlist[1];

            switchScene(name);
        }
        else if (eventId == Event_ToggleMainForm)
        {
            showMainForm(varlist[0]);
        }
    }


}