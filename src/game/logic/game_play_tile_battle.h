#pragma once

#include "game_play.h"

namespace game
{
	enum class GameTurnType 
	{
		Fighting,
		Moving,
	};

	class GamePlayTileBattle : public GamePlay, public signals::SlotHandler
	{
		using EntitySet = std::set<entt::entity>;
		
	public:
		GamePlayTileBattle(GameContext& context);
		~GamePlayTileBattle() = default;

		GamePlayType getType() override { return GamePlayType::GamePlay_TileBattle; };
		GameTurnType getTurn() { return _turnType; }

		void update(float deltaTime) override;
		void draw() override;
		
		entt::entity getSelectEntity() override;

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

		void onMouseLeftPressed(const Vec2& pos);
		void onMouseLeftRelease(const Vec2& pos);
		void onMouseLeftDrag(const Vec2& pos, const Vec2& offset);
		void onMouseMotion(const Vec2& pos, const Vec2& offset);
		
		void onKeyDown(KeyCode key);
		
	private:
		void switchGameTurn();

		bool isFightTurnOver();
		bool isMotionTurnOver();
		
		void onMoveStep(const Vec2i& dir);
		void onSkipMove();

		void unselectAll();
		void onSelectChange(const EntitySet& selectEntities);
		bool canSelectEntity(entt::entity entity);

		void startAutoFightFlow(entt::entity actor);

		EntitySet getMovableSelectedEntities(const Vec2i& dir);
		
	private:
		GameTurnType _turnType = GameTurnType::Moving;
		Rect _selectRect;
		EntitySet _selectEntities;
	};
}