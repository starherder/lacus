#include "game/logic/game_play_tile_battle.h"
#include "game/ecs/comm_event.h"
#include "magic_enum/magic_enum.h"

namespace game
{
	GamePlayTileBattle::GamePlayTileBattle(GameContext& context) : GamePlay(context)
	{
	}
	
	void GamePlayTileBattle::onKeyDown(KeyCode key)
	{
		
		switch (key)
		{
		case SDLK_W: return onMoveStep({0, -1});
		case SDLK_S: return onMoveStep({0, 1});
		case SDLK_A: return onMoveStep({-1, 0});
		case SDLK_D: return onMoveStep({1, 0});
		case SDLK_SPACE: return onSkipMove();
		default: return;
		}
	}
	
	void GamePlayTileBattle::onMoveStep(const Vec2i& dir)
	{
		if (!context().registry().valid(selectEntity())) 
		{ 
			return; 
		}

		if (!context().registry().try_get<CompMoveCfg>(selectEntity()))
		{
			return;
		}

		uint8_t moveGrids = 1;
		context().dispatcher().trigger(EvtStepMove{ selectEntity(), dir, moveGrids });
	}

	void GamePlayTileBattle::onSkipMove()
	{
		auto pGameTurn = context().registry().try_get<CompGameTurn>(selectEntity());
		if (pGameTurn && _turnType == GameTurnType::Moving)
		{
			pGameTurn->running = false;
		}
	}
	
	bool GamePlayTileBattle::isMoveStage()
	{
		return _turnType == GameTurnType::Moving;
	}
	
	bool GamePlayTileBattle::isFightStage()
	{
		return _turnType == GameTurnType::Fighting;
	}
	
	bool GamePlayTileBattle::isFightTurnOver()
	{
		auto entviews = context().registry().view<CompGameTurn>();
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

	bool GamePlayTileBattle::isMotionTurnOver()
	{
		auto entviews = context().registry().view<CompGameTurn>();
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
	
	void GamePlayTileBattle::update(float deltaTime)
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

	void GamePlayTileBattle::switchGameTurn()
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

		auto entviews = context().registry().view<CompGameTurn>();
		for (auto& actor : entviews)
		{
			auto& gameTurn = entviews.get<CompGameTurn>(actor);
			gameTurn.running = true;

			if (_turnType == GameTurnType::Fighting)
			{
				startAutoFightFlow();
			}
		}
	}

	void GamePlayTileBattle::onFightStart(entt::entity actor)
	{
	}
	
	void GamePlayTileBattle::onFightFinish(entt::entity actor)
	{
		auto& turn = context().registry().get<CompGameTurn>(actor);
		turn.running = false;
	}
		
	void GamePlayTileBattle::onMotionStart(entt::entity actor)
	{
	}
	
	void GamePlayTileBattle::onMotionFinish(entt::entity actor)
	{
		auto& turn = context().registry().get<CompGameTurn>(actor);
		turn.running = false;
	}

	void GamePlayTileBattle::startAutoFightFlow()
	{
		context().dispatcher().trigger(EvtRoleAutoAttack{ selectEntity() });		
	}
	
}