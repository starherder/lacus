#pragma once

#include "game/scene/game_context.h"

namespace game
{
	enum GamePlayType : int8_t
	{
		GamePlay_TileBattle = 0,
		GamePlay_AutoChess = 1,
	};

	class GamePlay
	{
	public:
		GamePlay() = delete;
		GamePlay(const GamePlay&) = delete;
		GamePlay(GamePlay&&) = delete;

		GamePlay(GameContext& context) : _context(context) {}
		virtual ~GamePlay() = default;

		virtual GamePlayType getType() = 0;

		GameContext& context() { return _context; }

		virtual entt::entity getSelectEntity() { return entt::null; }

	public:
		virtual void update(float deltaTime) = 0;
		virtual void draw() = 0;

		virtual bool isTileBattle() { return false; }
		virtual bool isAutoChess() { return false; }
		
		virtual bool isMoveStage() { return true; }
		virtual bool isFightStage() { return true; } 

		virtual void onActorCreate(entt::entity actor) {}
		virtual void onActorDestroy(entt::entity actor) {}
		
		virtual void onFightStart(entt::entity actor) {}
		virtual void onFightFinish(entt::entity actor) {}
		
		virtual void onMotionStart(entt::entity actor) {}
		virtual void onMotionFinish(entt::entity actor) {}
		
	protected:	
		GameContext& _context;
	};
	
}