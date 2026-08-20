#pragma once

#include "engine/application.h"
#include "bevtree/bevtree.h"
#include "ui/gui_manager.h"
#include "a_star/a_star.hpp"

namespace game {

	using namespace ui;
	using namespace engine;
	using namespace bevtree;

	class GameScript;
	class GameConfig;
	class LogicConfig;
	class SceneConfig;
	class GameScene;
	class GameCamera;
	class GameData;
	class GamePlay;
	class GameLogic;
	class ObjectManager;

	class GameContext final
	{
	public:
		using PathFinder = AStar::Generator;
		using OptPathList = std::optional<AStar::CoordinateList>;

	public:
		GameContext() = delete;
		GameContext(GameContext&&) = delete;
		GameContext(const GameContext&) = delete;

		GameContext(Application& app);
		~GameContext() {}

		Application& applicaton() { return _application; }
		SystemConfig& systemConfig() { return _application.systemConfig(); }

		Window& window() { return _application.window(); }
		Renderer& renderer() { return _application.renderer(); }
		IPainter& painter() { return _application.painter(); }
		AudioPlayer& audioPlayer() { return _application.audioPlayer(); }

		ResourceManager& resourceMgr() { return _application.resourceManager(); }
		TextureManager& textureMgr() { return resourceMgr().textureManager(); }
		FontManager& fontMgr() { return resourceMgr().fontManager(); }
		AudioManager& audioMgr() { return resourceMgr().audioManager(); }

	    const FrameTicker& frameTicker() { return _application.frameTicker(); }
		EventDispatcher& eventDispatcher() { return _application.eventDispatcher(); }

		BevTreeManager& bevtreeMgr() { return BevTreeManager::inst(); }
		GuiManager& guiMgr() { return GuiManager::inst(); }

		const fs::path& resPath() { return _application.resPath(); }

		GameScene& scene() { return *_currentScene;  }
		void setScene(GameScene* scene) { _currentScene = scene; }

		GameConfig& gameConfig() { return *_gameConfig; }
		void setGameConfig(GameConfig* config) { _gameConfig = config; }

		SceneConfig& sceneConfig() { return *_sceneConfig; }
		void setSceneConfig(SceneConfig* config) { _sceneConfig = config; }

		GamePlay& gamePlay();
		void setGamePlay(GamePlay* gamePlay);

		GameLogic& gameLogic() { return *_gameLogic; }
		void setGameLogic(GameLogic* gameLogic) { _gameLogic = gameLogic; }

		LogicConfig& logicConfig() { return *_logicConfig; }
		void setLogicConfig(LogicConfig* config) { _logicConfig = config; }

		GameScript& gameScript() { return *_gameScript; }
		void setGameScript(GameScript* script) { _gameScript = script; }

		int64_t currentTicks() { return _application.frameTicker().ticks(); }
		int deltaTicks() { return _application.frameTicker().deltaTicks(); }

		ObjectManager& objectManager();

		entt::registry& registry();
		entt::dispatcher& dispatcher();

		GameCamera& camera();
		GameData& dataCenter();

		OptPathList findPath(const Vec2i& srcGrid, const Vec2i& dstGrid, bool can_swim);

		PathFinder& pathFinder() { return _pathFinder; }
		PathFinder& landPathFinder() { return _landPathFinder; }

		bool debugMode() { return _debugMode; }
		void setDebugMode(bool debug) { _debugMode = debug; }

		void pushGamePause() { _gamePauseCount++; }
		void popGamePause() { if (_gamePauseCount > 0) { _gamePauseCount--; } }
		bool gamePaused() const { return _gamePauseCount > 0; }

	private:
		Application& _application;

		GameScene* _currentScene = nullptr;

		GameConfig* _gameConfig = nullptr;
		
		SceneConfig* _sceneConfig = nullptr;

		GameScript* _gameScript = nullptr;

		LogicConfig* _logicConfig = nullptr;

		GamePlay* _gamePlay = nullptr;

		GameLogic* _gameLogic = nullptr;

		PathFinder _pathFinder;

		PathFinder _landPathFinder;

		bool _debugMode = false;

		int _gamePauseCount = 0;
	};




}


