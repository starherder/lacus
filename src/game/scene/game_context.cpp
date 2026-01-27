#include "game_context.h"
#include "game/scene/game_camera.h"
#include "game/scene/game_scene.h"
#include "game/scene/object_factory.h"

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


	ObjectFactory& GameContext::objectFactory()
	{
		return ObjectFactory::inst();
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

}