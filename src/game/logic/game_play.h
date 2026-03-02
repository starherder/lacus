#pragma once

#include "game/scene/game_context.h"
#include "game/ecs/comm_comp.h"

namespace game
{

	class GamePlay final
	{
	public:
		GamePlay() = delete;
		GamePlay(const GamePlay&) = delete;
		GamePlay(GamePlay&&) = delete;

		GamePlay(GameContext& context);

		GameTurnType currentTurnType() { return _turnType; }

		void onUpdate(float deltaTime);

	private:
		void switchGameTurn();

	private:
		GameContext& _context;

		GameTurnType _turnType = GameTurnType::Fighting;
	};
}