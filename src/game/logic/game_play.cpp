#include "game/logic/game_play.h"
#include "game/ecs/comm_event.h"
#include "magic_enum/magic_enum.h"

namespace game
{

	GamePlay::GamePlay(GameContext& context) : _context(context)
	{

	}

	void GamePlay::onUpdate(float deltaTime)
	{

		// 有问题啊： 没有任何操作的时候，会不停的反转

		bool allTurnOver = true;

		auto entviews = _context.registry().view<CompGameTurn>();
		for (auto& actor : entviews)
		{
			auto& gameTurn = entviews.get<CompGameTurn>(actor);
			
			if (gameTurn.running)
			{
				allTurnOver = false;
				break;
			}
		}

		if (allTurnOver)
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