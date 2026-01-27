#pragma once

#include "game/scene/game_context.h"

#include "game/ecs/comm_comp.h"
#include "game/ecs/comm_system.h"
#include "game/ecs/comm_event.h"


namespace game
{
	enum GameState {
		Running,
		Pause,
		Finish,
	};

	class GameLogic : public signals::SlotHandler
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

		void startFirstScene();

		bool switchScene(const std::string& sceneName);

	private:
		void onSceneLoadProgress(float progress);

		void onStartNewGame();
		void onResumeGame();
		void onConfigGame();
		void onExitGame();

		void showLoadingForm(bool visible);
		void showMainForm(bool visible);

		void onUICustomEvent(int eventId, const utility::VarList& varlist);

		void initEscSystem();

		void onDebugReloadResource();

		void closeAllForms();
		
		void checkGameState();

	private:
		std::unique_ptr<GameScene> _scene = nullptr;

		GameContext& _gameContext;

		GameConfig _gameConfig;

		GameState _state = GameState::Running;
	};
}