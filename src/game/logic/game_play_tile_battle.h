#pragma once

#include "game_play.h"

namespace game
{
	enum class GameTurnType 
	{
		Fighting,
		Moving,
	};

	class GamePlayTileBattle : public GamePlay
	{
	public:
		GamePlayTileBattle(GameContext& context);
		~GamePlayTileBattle() = default;
		
		void update(float deltaTime) override;
		
	public:
		bool isMoveStage() override;
		bool isFightStage() override; 
		
		bool isTileBattle() override { return true; }

		void onActorCreate(entt::entity actor) override;
		void onActorDestroy(entt::entity actor) override;

		void onFightStart(entt::entity actor) override;
		void onFightFinish(entt::entity actor) override;
		
		void onMotionStart(entt::entity actor) override;  
		void onMotionFinish(entt::entity actor) override;

		void onKeyDown(KeyCode key) override;
		
	private:
		void switchGameTurn();

		bool isFightTurnOver();
		bool isMotionTurnOver();
		
		void onMoveStep(const Vec2i& dir);
		void onSkipMove();

		void startAutoFightFlow(entt::entity actor);
		
	private:
		GameTurnType _turnType = GameTurnType::Moving;
	};
}