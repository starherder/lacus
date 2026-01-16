#pragma once

#include "game/scene/game_context.h"


namespace game
{

	class GameLogic : public signals::SlotHandler
	{
	public:
		GameLogic() = delete;
		GameLogic(const GameLogic&) = delete;
		GameLogic(GameLogic&&) = delete;

		GameLogic(GameContext& context);
		~GameLogic();

		void loadResource();

		void showLoadingForm(bool visible);

		void startFirstScene();

		bool switchScene(const std::string& sceneName);


		void init();
		void shut();
		void start();
		void pause();

		void update(float delta);
		void draw();

	private:
		void onSceneLoadProgress(float progress);
		void onStartNewGame();
		void onResumeGame();
		void onConfigGame();
		void onExitGame();

	private:

		std::unique_ptr<GameScene> _scene = nullptr;

		GameContext& _gameContext;

		GameConfig _gameConfig;
	};
}