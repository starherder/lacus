#include "game_context.h"
#include "game/scene/game_camera.h"
#include "game/scene/game_scene.h"

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

	entt::registry& GameContext::registry()
	{
		return currentScene().registry();
	}

	GameCamera& GameContext::camera() 
	{ 
		return currentScene().camera(); 
	}

}