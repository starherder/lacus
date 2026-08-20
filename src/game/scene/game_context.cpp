#include "game_context.h"
#include "game/scene/game_camera.h"
#include "game/scene/game_scene.h"
#include "game/scene/object_manager.h"
#include "game/logic/game_play_tile_battle.h"


namespace game 
{

	GameContext::GameContext(Application& app) : _application(app)
	{
		_debugMode = app.systemConfig().debug_mode;
	}

	entt::dispatcher& GameContext::dispatcher()
	{
		return eventDispatcher().logicDispatcher();
	}


	ObjectManager& GameContext::objectManager()
	{
		return ObjectManager::inst();
	}

	entt::registry& GameContext::registry()
	{
		return scene().registry();
	}

	GameCamera& GameContext::camera() 
	{ 
		return scene().camera(); 
	}

	GameData& GameContext::dataCenter()
	{
		return _currentScene->dataCenter(); 
	}

	GamePlay& GameContext::gamePlay()
	{
		return *_gamePlay;
	}

	void GameContext::setGamePlay(GamePlay* gamePlay)
	{
		_gamePlay = gamePlay;
	}

	GameContext::OptPathList GameContext::findPath(const Vec2i& srcGrid, const Vec2i& dstGrid, bool can_swim)
	{
		if (can_swim)
		{
			return pathFinder().findPath(srcGrid, dstGrid);
		}
		else
		{
			return landPathFinder().findPath(srcGrid, dstGrid);
		}
	}

}