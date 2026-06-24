#include "game/logic/game_logic.h"

#include "bevtree/bevtree.h"
#include "utility/translator.h"

#include "game/ecs/system_utils.h"

#include "game/scene/game_scene.h"
#include "game/scene/object_factory.h"

#include "game/ui/form_loading.h"
#include "game/ui/form_start.h"
#include "game/ui/form_main.h"
#include "game/ui/form_scenes.h"
#include "game/ui/ui_logic_events.h"
#include "game/ui/form_main.h"
#include "game/ui/imform_debug.h"
#include "game/ui/form_result.h"
#include "game/ui/form_chess_tip.h"

#include "game/logic/game_play_tile_battle.h"

#include "engine/animation.h"

namespace game
{
	GameLogic::GameLogic(GameContext& context):_context(context)
	{
		_scene = std::make_unique<GameScene>(_context);
        _scene->on_load_progress.connect(this, &GameLogic::onSceneLoadProgress);
        _scene->on_hover_object.connect(this, &GameLogic::onSceneObjectHover);
        _scene->on_leave_object.connect(this, &GameLogic::onSceneObjectLeave);

		_context.setScene(_scene.get());
		_context.setGameConfig(&_gameConfig);
        _context.setGameScript(&_gameScript);
	    _context.setSceneConfig(&_sceneConfig);
		_context.setLogicConfig(&_logicConfig);

        _gameScript.load(_context.resPath() / _gameConfig.script.entry);

        GuiManager().inst().on_custom_event.connect(this, &GameLogic::onUICustomEvent);

        initEscSystem();
	}

	GameLogic::~GameLogic()
	{
	}

    void GameLogic::initEscSystem()
    {
        EcsSystemManager::inst().init(_context);
    }
    
	void GameLogic::loadResource()
	{
        ObjectFactory::inst().init(&_context);

        auto textdir = _context.resPath() / "localized/CHS/";
        auto res = utility::StringTranslator::inst().load(utility::Language::SimpleChinese, textdir);
        if (!res) 
        {
            LogError("load translator file ({}) failed.", textdir.string());
        }

	    auto sceneCfg = _context.resPath() / "scenes/scenes.json";
		res = _sceneConfig.load(sceneCfg);
		if (!res) 
        {
			LogError("load scene config: {} failed.", sceneCfg.string());
		}
	    
        auto gamecfg = _context.resPath() / "game_config.json";
        res = _gameConfig.load(gamecfg);
        if (!res) 
        {
            LogError("load game config: {} failed.", gamecfg.string());
        }

        auto emotioncfg = _context.resPath() / "data/emotion/emotions.json";
        res = _logicConfig.loadEmotion(emotioncfg);
        if (!res)
        {
            LogError("load emotion config: {} failed.", emotioncfg.string());
        }
        
        auto btreePath =_context.resPath() / "data/bevtree/";
        res = bevtree::BevTreeManager::inst().load(btreePath);
        if (!res) 
        {
            LogError("load bevtree config: {} failed.", btreePath.string());
        }

        auto roleCfgs =_context.resPath() / "data/role/";
        res = ObjectFactory::inst().loadRoles(roleCfgs);
        if (!res) 
        {
            LogError("load role config: {} failed.", roleCfgs.string());
        }
/*
        auto enemyCfgs = _context.resPath() / "data/role/";
        res = ObjectFactory::inst().loadEnemies(enemyCfgs);
        if (!res) 
        {
            LogError("load enemy config: {} failed.", enemyCfgs.string());
        }
*/
        auto otherCfgs = _context.resPath() / "data/other/";
        res = ObjectFactory::inst().loadOther(otherCfgs);
        if (!res) 
        {
            LogError("load other config: {} failed.", otherCfgs.string());
        }

        auto itemCfgs =_context.resPath() / "data/item/";
        res = ObjectFactory::inst().loadItems(itemCfgs);
        if (!res) 
        {
            LogError("load item config: {} failed.", itemCfgs.string());
        }

        auto skilldir =_context.resPath() / "data/skill/";
        res = ObjectFactory::inst().loadSkills(skilldir);
        if (!res) 
        {
            LogError("load skill config: {} failed.", skilldir.string());
        }

        auto buffdir =_context.resPath() / "data/buff/";
        res = ObjectFactory::inst().loadBuffs(buffdir);
        if (!res) 
        {
            LogError("load buff config: {} failed.", buffdir.string());
        }

        auto particleCfgs =_context.resPath() / "particles/";
        res = particle::ParticleManager::inst().LoadParticles(particleCfgs);
        if (!res) 
        {
            LogError("load partiles config: {} failed.", particleCfgs.string());
        }


        auto texturedir = _context.resPath() / "textures";
        _context.textureMgr().loadAllTexSets(texturedir);

        auto animDir = _context.resPath() / "animations";
        _context.resourceMgr().animationManager().loadAll(animDir, _context.textureMgr());
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
        auto formStart = ui::GuiManager::inst().createForm<FormStart>("form_start", _context);
        formStart->on_start_game.connect(this, &GameLogic::onStartNewGame);
        formStart->on_resume_game.connect(this, &GameLogic::onResumeGame);
        formStart->on_config_game.connect(this, &GameLogic::onConfigGame);
        formStart->on_exit_game.connect(this, &GameLogic::onExitGame);
        
        auto form_debug = imgui::ImFormManager::inst().showForm<ImFormDebug>("ImFormDebug");
        if (form_debug)
        {
            form_debug->init(&_context);
            form_debug->on_reload_res.connect(this, &GameLogic::onDebugReloadResource);
            form_debug->on_reload_script.connect(this, &GameLogic::onDebugReloadScript);
        }
    }

    void GameLogic::checkGameState()
    {
        if (gameMode() == SceneGameMode::GameMode_Test)
        {
            return;
        }

        if (_state != GameState::Running)
        {
            return;
        }

        int liveRole = 0;
        auto views = _context.registry().view<CompComm>();
        for (auto& ent : views)
        {
            auto pdead = _context.registry().try_get<CompDead>(ent);
            if (pdead) {
                continue;
            }

            auto& comp = views.get<CompComm>(ent);
            if (comp.side == CampSide::Gangster) {
                liveRole++;
            }
        }

        int handCards = (int)_context.scene().dataCenter().getHandCards().size();

        if (liveRole == 0 && handCards == 0)
        {
            _state = GameState::Finish;
            auto result = GameResult::Failed;

            ui::GuiManager::inst().createForm<FormResult>("form_result", _context, result);
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

        //showLoadingForm(true);

        showScenesForm();

        _scene->onStart();
    }

    bool GameLogic::switchScene(const std::string& sceneId)
    {
        _currentScene = sceneId;
        _state = GameState::Running;

        closeAllForms();

        showLoadingForm(true);

        _scene->unload();

        _scene->load(sceneId);

        return true;
    }

    bool GameLogic::restartScene()
    {
        _context.dataCenter().clearHandCard();

        auto& cardGroup = _context.dataCenter().getCardGroup();
        for (auto& card : cardGroup)
        {
            _context.dataCenter().addHandCard(card);
        }

        return switchScene(_currentScene);
    }

    void GameLogic::showScenesForm()
    {
        auto form = GuiManager::inst().getForm<FormScenes>("form_scenes");
        if (form)
        {
            form->setVisible(true);
        }
        else
        {
            ui::GuiManager::inst().createForm<FormScenes>("form_scenes", _context);
        }
    }

    void GameLogic::showMainForm(bool visible)
    {
        if (visible)
        {
            ui::GuiManager::inst().createForm<FormMain>("form_main", _context);

            auto form = ui::GuiManager::inst().createForm<FormChessTip>("form_chess_tip", _context);
            if (form)
            {
                form->setVisible(false);
            }
        }
        else
        {
            ui::GuiManager::inst().closeForm("form_main");
            ui::GuiManager::inst().closeForm("form_chess_tip");
        }

    }

    void GameLogic::showLoadingForm(bool visible)
    {
        if (visible)
        {
            ui::GuiManager::inst().createForm<FormLoading>("form_loading", _context);
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
        switch (eventId)
        {
        case Event_SelectScene: 
        {
            switchScene(varlist[0]);
            int mode = varlist[1];
            setGameMode((SceneGameMode)mode);
        }break;
        case Event_ToggleMainForm: 
        {
            showMainForm(varlist[0]);
        }break;
        case Event_ShowScenes: 
        {
            showScenesForm();
        }break;
        case Event_ReplayLevel: 
        {
            restartScene();
        }break;
        default: 
        {
        }
        }
    }

    void GameLogic::onDebugReloadScript()
    {
        _gameScript.reload();
    }

    void GameLogic::onDebugReloadResource()
    {
        ObjectFactory::inst().reloadAll();
    }

    void GameLogic::closeAllForms()
    {
        ui::GuiManager::inst().closeAllForms();
    }

    void GameLogic::onSceneObjectHover(entt::entity obj)
    {
        auto comm = _context.registry().try_get<CompComm>(obj);
        if (comm && comm->type == ObjectType::Npc)
        {
            auto form = ui::GuiManager::inst().getForm<FormChessTip>("form_chess_tip");
            if (form)
            {
                auto mousePos = _context.eventDispatcher().mousePos();
                form->setPos(mousePos + Vec2{ 10, 10 });

                form->setVisible(true);
                form->showChessTip(obj);
            }
        }
    }
    
    void GameLogic::onSceneObjectLeave(entt::entity obj)
    {
        auto comm = _context.registry().try_get<CompComm>(obj);
        if (comm && comm->type == ObjectType::Npc)
        {
            auto form = ui::GuiManager::inst().getForm<FormChessTip>("form_chess_tip");
            if (form)
            {
                form->setVisible(false);
            }
        }
    }

}