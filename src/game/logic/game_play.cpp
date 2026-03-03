#include "game/logic/game_play.h"
#include "game/ecs/comm_event.h"
#include "magic_enum/magic_enum.h"

namespace game
{

	GamePlay::GamePlay(GameContext& context) : _context(context)
	{

	}

	bool GamePlay::isFightTurnOver()
	{
		auto entviews = _context.registry().view<CompGameTurn>();
		for (auto& actor : entviews)
		{
			auto& gameTurn = entviews.get<CompGameTurn>(actor);
			if (gameTurn.running)
			{
				return false;
			}
		}

		return true;
	}

	bool GamePlay::isMotionTurnOver()
	{
		auto entviews = _context.registry().view<CompGameTurn>();
		for (auto& actor : entviews)
		{
			auto& gameTurn = entviews.get<CompGameTurn>(actor);
			if (!gameTurn.running)
			{
				return true;
			}
		}

		return false;
	}
	
	void GamePlay::onUpdate(float deltaTime)
	{
		bool isTurnOver = false;

		if (_turnType == GameTurnType::Fighting)
		{
			isTurnOver = isFightTurnOver();
		}
		else
		{
			isTurnOver = isMotionTurnOver();
		}

		if (isTurnOver)
		{
			switchGameTurn();
		}
	}

	void GamePlay::switchGameTurn()
	{
		if (_turnType == GameTurnType::Moving) 
		{
			_turnType = GameTurnType::Fighting;
		}
		else 
		{
			_turnType = GameTurnType::Moving;
		}

		LogInfo("swith turn to : {}", MagicEnumText(GameTurnType, _turnType));

		auto entviews = _context.registry().view<CompGameTurn>();
		for (auto& actor : entviews)
		{
			auto& gameTurn = entviews.get<CompGameTurn>(actor);
			gameTurn.running = true;

			_context.dispatcher().trigger(EvtGameTurnStart{ actor, _turnType });
		}
	}
}