#pragma once

#include "engine/application.h"
#include "bevtree/bevtree.h"
#include "ui/gui_manager.h"
#include "a_star/a_star.hpp"
#include "game/game_config.h"

namespace game {

	using namespace ui;
	using namespace engine;
	using namespace bevtree;

	class GameScene;
	class GameCamera;
	class ObjectFactory;

	class GameContext
	{
	public:
		using PathFinder = AStar::Generator;

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
		ImPainter& imPainter() { return _application.im_painter(); }
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

		GameScene& currentScene() { return *_currentScene;  }
		void setCurrentScene(GameScene* scene) { _currentScene = scene; }

		GameConfig& gameConfig() { return *_gameConfig; }
		void setGameConfig(GameConfig* config) { _gameConfig = config; }

		ObjectFactory& objectFactory();

		entt::registry& registry();
		entt::dispatcher& dispatcher();

		GameCamera& camera();
		PathFinder& pathFinder() { return _pathFinder; }

		Painter& painter() { return _application.painter(); }

		bool debugMode() { return _debugMode; }
		void setDebugMode(bool debug) { _debugMode = debug; }

	private:
		Application& _application;

		GameScene* _currentScene = nullptr;

		GameConfig* _gameConfig = nullptr;

		PathFinder _pathFinder;

		bool _debugMode = false;
	};




}


