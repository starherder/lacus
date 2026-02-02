#pragma once

#include "game/scene/game_context.h"

#include "game/ecs/comm_comp.h"
#include "game/ecs/comm_system.h"
#include "game/ecs/comm_event.h"

#include "game/game_config.h"
#include "game/game_script.h"



namespace game
{
	enum GameState {
		Running,
		Pause,
		Finish,
	};

	class GameLogic final : public signals::SlotHandler
	{
	public:
		using EcsSystemMap = std::multimap<EcsPriority, std::shared_ptr<EcsSystem>>;

	public:
		GameLogic() = delete;
		GameLogic(const GameLogic&) = delete;
		GameLogic(GameLogic&&) = delete;

		GameLogic(GameContext& context);
		~GameLogic();

		void init();
		void shut();
		void start();
		void pause();

		void update(float delta);
		void draw();

		void loadResource();

		bool switchScene(const std::string& sceneName);
		bool restartScene();

	private:
		void startFirstScene();

		void onSceneLoadProgress(float progress);

		void onStartNewGame();
		void onResumeGame();
		void onConfigGame();
		void onExitGame();

		void showLoadingForm(bool visible);
		void showMainForm(bool visible);
		void showScenesForm();

		void onUICustomEvent(int eventId, const utility::VarList& varlist);

		void initEscSystem();

		void onDebugReloadResource();
		
		void onDebugReloadScript();

		void closeAllForms();
		
		void checkGameState();

		void onSceneObjectHover(entt::entity obj);
		void onSceneObjectLeave(entt::entity obj);
		void onSceneObjectSelect(entt::entity obj);

	private:
		std::string _currentScene;

		std::unique_ptr<GameScene> _scene = nullptr;

		GameContext& _context;

		GameConfig _gameConfig;

		GameScript _gameScript;

		GameState _state = GameState::Running;
	};
}