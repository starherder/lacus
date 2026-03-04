#pragma once

#include "game/scene/game_context.h"

namespace game
{
	class GamePlay
	{
	public:
		GamePlay() = delete;
		GamePlay(const GamePlay&) = delete;
		GamePlay(GamePlay&&) = delete;

		GamePlay(GameContext& context) : _context(context) {}
		virtual ~GamePlay() = default;

		GameContext& context() { return _context; }

		void setSelectEntity(entt::entity ent) { _selectEntity = ent; }
		entt::entity selectEntity() { return _selectEntity; }

	public:
		virtual void update(float deltaTime) = 0;

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
		
		virtual void onKeyDown(KeyCode key) {}
		
	private:	
		GameContext& _context;

		entt::entity _selectEntity = entt::null;
	};
	
}