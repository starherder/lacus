#include "game/logic/game_logic.h"

#include "bevtree/bevtree.h"
#include "utility/translator.h"

#include "game/ecs/system_utils.h"

#include "game/scene/game_scene.h"
#include "game/scene/object_manager.h"
#include "game/scene/save_manager.h"
#include "game/scene/story_manager.h"
#include "game/scene/dialog_manager.h"

#include "game/ui/form_loading.h"
#include "game/ui/form_start.h"
#include "game/ui/form_main.h"
#include "game/ui/form_scenes.h"
#include "game/ui/ui_logic_events.h"
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
		_context.setGameLogic(this);
		_context.setGameConfig(&_gameConfig);
        _context.setGameScript(&_gameScript);
	    _context.setSceneConfig(&_sceneConfig);
		_context.setLogicConfig(&_logicConfig);
        _context.setGameData(&_gameData);

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
        ObjectManager::inst().init(&_context);
        StoryManager::inst().init(&_context);
        DialogManager::inst().init(&_context);

        auto textdir = _context.resPath() / "localized/CHS/";
        auto res = utility::StringTranslator::inst().load(utility::Language::SimpleChinese, textdir);
        if (!res) 
        {
            LogError("load translator file ({}) failed.", textdir.string());
        }

        auto storydir = _context.resPath() / "data/story/";
        res = StoryManager::inst().load(storydir);
        if (!res)
        {
            LogError("load story config: {} failed.", storydir.string());
        }

        auto dialogdir = _context.resPath() / "data/dialog/";
        res = DialogManager::inst().load(dialogdir);
        if (!res)
        {
            LogError("load dialog config: {} failed.", dialogdir.string());
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
        res = ObjectManager::inst().loadRoles(roleCfgs);
        if (!res) 
        {
            LogError("load role config: {} failed.", roleCfgs.string());
        }
/*
        auto enemyCfgs = _context.resPath() / "data/role/";
        res = ObjectManager::inst().loadEnemies(enemyCfgs);
        if (!res) 
        {
            LogError("load enemy config: {} failed.", enemyCfgs.string());
        }
*/
        auto otherCfgs = _context.resPath() / "data/other/";
        res = ObjectManager::inst().loadOther(otherCfgs);
        if (!res) 
        {
            LogError("load other config: {} failed.", otherCfgs.string());
        }

        auto itemCfgs =_context.resPath() / "data/item/";
        res = ObjectManager::inst().loadItems(itemCfgs);
        if (!res) 
        {
            LogError("load item config: {} failed.", itemCfgs.string());
        }

        auto skilldir =_context.resPath() / "data/skill/";
        res = ObjectManager::inst().loadSkills(skilldir);
        if (!res) 
        {
            LogError("load skill config: {} failed.", skilldir.string());
        }

        auto buffdir =_context.resPath() / "data/buff/";
        res = ObjectManager::inst().loadBuffs(buffdir);
        if (!res) 
        {
            LogError("load buff config: {} failed.", buffdir.string());
        }

        auto particleCfgs =_context.resPath() / "particles/";
        res = particle::ParticleManager::inst().loadParticles(particleCfgs);
        if (!res) 
        {
            LogError("load partiles config: {} failed.", particleCfgs.string());
        }


        auto texturedir = _context.resPath() / "textures";
        _context.textureMgr().loadAllTexSets(texturedir);

        auto animDir = _context.resPath() / "animations";
        _context.resourceMgr().animationManager().loadAll(animDir);

        auto savePath = _context.resPath() / "save/game_data.json";
        if (fs::exists(savePath))
        {
            SaveManager::inst().loadGameData(savePath, _context.gameData());
        }

        if (!_context.gameData().hasUnlockedCardData())
        {
            _context.gameData().unlockCards(ObjectManager::inst().getAllRoleCfgIds());
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

        if (!_context.gamePaused())
        {
            _scene->onUpdate(delta);

            checkGameState();
        }

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
        auto form_debug = imgui::ImFormManager::inst().showForm<ImFormDebug>("ImFormDebug");
        if (form_debug)
        {
            form_debug->init(&_context);
            form_debug->on_reload_res.connect(this, &GameLogic::onDebugReloadResource);
            form_debug->on_reload_script.connect(this, &GameLogic::onDebugReloadScript);
        }

        ui::GuiManager::inst().showForm<FormStart>("form_start", _context);
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

        if (liveRole == 0 )
        {
            finishGame(GameResult::Failed);
        }
    }

    void GameLogic::finishGame(GameResult result)
    {
        if (_state == GameState::Finish)
        {
            return;
        }

        if (result == GameResult::Success && !_currentScene.empty())
        {
            _context.gameData().setSceneCleared(_currentScene);
            SaveManager::inst().saveGameData(_context.resPath() / "save/game_data.json", _context.gameData());
        }

        _state = GameState::Finish;
        ui::GuiManager::inst().showForm<FormResult>("form_result", _context, result);
    }

    void GameLogic::pause()
    {
        imgui::ImFormManager::inst().closeForm("ImFormDebug");
    }

    void GameLogic::startFirstScene()
    {
        closeAllForms();

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
            ui::GuiManager::inst().showForm<FormScenes>("form_scenes", _context);
        }
    }

    void GameLogic::showMainForm(bool visible)
    {
        if (visible)
        {
            ui::GuiManager::inst().showForm<FormMain>("form_main", _context);

            auto form = ui::GuiManager::inst().showForm<FormChessTip>("form_chess_tip", _context);
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
            ui::GuiManager::inst().showForm<FormLoading>("form_loading", _context);
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
    }

    void GameLogic::onDebugReloadScript()
    {
        _gameScript.reload();
    }

    void GameLogic::onDebugReloadResource()
    {
        ObjectManager::inst().reloadAll();

        auto storydir = _context.resPath() / "data/story/";
        auto res = StoryManager::inst().load(storydir);
        if (!res)
        {
            LogError("load story config: {} failed.", storydir.string());
        }

        auto dialogdir = _context.resPath() / "data/dialog/";
        res = DialogManager::inst().load(dialogdir);
        if (!res)
        {
            LogError("load dialog config: {} failed.", dialogdir.string());
        }
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
