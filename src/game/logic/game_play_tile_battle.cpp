#include "game/logic/game_play_tile_battle.h"
#include "game/ecs/comm_event.h"
#include "game/scene/game_scene.h"
#include "magic_enum/magic_enum.h"

namespace game
{
	struct CompMoveBlocked
	{
		bool blocked = false;
	};
	
	GamePlayTileBattle::GamePlayTileBattle(GameContext& context) : GamePlay(context)
	{
		context.eventDispatcher().onMouseLeftDown.connect(this, &GamePlayTileBattle::onMouseLeftPressed, -1);
		context.eventDispatcher().onMouseLeftUp.connect(this, &GamePlayTileBattle::onMouseLeftRelease, -1);
		context.eventDispatcher().onMouseLeftDrag.connect(this, &GamePlayTileBattle::onMouseLeftDrag, -1);
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

	void GamePlayTileBattle::onMouseLeftPressed(const Vec2& pos)
	{
		auto scenePos = context().camera().screenToWorld(pos);
		_selectRect = Rect{ scenePos.x, scenePos.y, 0.0f, 0.0f };

		//unselectAll();

		auto selent = context().scene().findObjectAtPos(_selectRect.pos());
		if (context().registry().valid(selent))
		{
			onSelectChange({ selent });
		}
	}

	void GamePlayTileBattle::onMouseLeftRelease(const Vec2& pos)
	{
		_selectRect = { 0,0,0,0 };
	}
	
	void GamePlayTileBattle::onMouseLeftDrag(const Vec2& pos, const Vec2& offset)
	{
		auto scenePos = context().camera().screenToWorld(pos);
		_selectRect.w = scenePos.x - _selectRect.x;
		_selectRect.h = scenePos.y - _selectRect.y;

		auto selents = context().scene().getObjectsInRect(_selectRect);
		onSelectChange(selents);
	}
	
	void GamePlayTileBattle::onMoveStep(const Vec2i& dir)
	{
		if (!isMoveStage())
		{
			return;
		}

		// dir 只支持 4 个方向
		if (dir.x * dir.x + dir.y * dir.y != 1)
		{
		    return;
		}
		
		checkMoveValid(dir);
		
		for(auto& entity : _selectEntities)
		{
			if (!context().registry().valid(entity))
			{
				continue;
			}

			if (context().registry().try_get<CompMoveCfg>(entity) == nullptr)
			{
				continue;
			}

			auto compBlock =context().registry().try_get<CompMoveBlocked>(entity); 
			if ( !compBlock )
			{
				uint8_t moveGrids = 1;
				context().dispatcher().trigger(EvtStepMove{ entity, dir, moveGrids });				
			}
			else
			{
				context().registry().remove<CompMoveBlocked>(entity);
			}
		}
	}

	void GamePlayTileBattle::checkMoveValid(const Vec2i& dir)
	{
		std::map<int, int> frontGridsX;
		std::map<int, int> frontGridsY;
		
		GridEntityMap allEntityGrids;
		
		for (auto& entity : _selectEntities)
		{
			if (!context().registry().valid(entity))
			{
				continue;
			}

			auto& transform = context().registry().get<CompTransform>(entity);
			auto grid = context().scene().getGridFromPos(transform.position);
			allEntityGrids.insert({ grid, entity });
			
			if (dir.x == 0)
			{
				auto it = frontGridsX.find(grid.x);
				if (it == frontGridsX.end())
				{
					frontGridsX.insert({grid.x, grid.y});
					continue;
				}

				if (dir.y > 0)
				{
					it->second = std::max(it->second, grid.y);
				}
				else if (dir.y < 0)
				{
					it->second = std::min(it->second, grid.y);
				}
				else
				{
					LogError("checkMoveValid: dir.x == 0 && dir.y == 0");
					continue;
				}
			}
			else if (dir.y == 0)
			{
				auto it = frontGridsY.find(grid.y);
				if (it == frontGridsY.end())
				{
					frontGridsY.insert({grid.y, grid.x});
					continue;
				}
				
				if (dir.x > 0)
				{
					it->second = std::max(it->second, grid.x);
				}
				else if (dir.x < 0)
				{
					it->second = std::min(it->second, grid.x);
				}
				else
				{
					LogError("checkMoveValid: dir.x == 0 && dir.y == 0");
					continue;
				}
			}
			else
			{
				LogError("checkMoveValid: dir.x != 0 && dir.y != 0");
				continue;
			}
		}

		std::map<int, int> frontGrids;
		if (dir.x == 0)
		{
			frontGrids = frontGridsX;
		}
		else if (dir.y == 0)
		{
			std::transform(frontGridsY.begin(), frontGridsY.end(),
				std::inserter(frontGrids, frontGrids.begin()),
				[](auto& it) { return std::make_pair(it.second, it.first); });
		}
		else
		{
			LogError("checkMoveValid: dir.x != 0 && dir.y != 0");
			return;
		}
		
		for (auto& it : frontGrids)
		{
			auto grid = Vec2i{ it.first, it.second };
			auto nextGrid = grid + dir;

			auto walkType = context().scene().getGridWalkType(nextGrid);
			
			bool blocked = walkType == (int)tilemap::WalkType::Collision;
			blocked = blocked || context().scene().hasObjectInGrid(nextGrid, ObjectType::Npc);
			
			if (blocked)
			{
				LogError("checkMoveValid: walkType == tilemap::WalkType::Collision");

				blockOneLine(allEntityGrids, grid, dir);
				return;
			}
		}
	}

	void GamePlayTileBattle::blockOneLine(const GridEntityMap& allEntityGrids, const Vec2i& grid, const Vec2i& dir)
	{
		auto gridindex = grid;
		
		while (true)
		{
			auto it = allEntityGrids.find(gridindex);
			if (it == allEntityGrids.end())
			{
				return;
			}

			auto entity = it->second;
			context().registry().emplace<CompMoveBlocked>(entity);

			gridindex -= dir;
		}
	}
	
	void GamePlayTileBattle::unselectAll()
	{
		auto selviews = context().registry().view<CompSelection>();
		for (auto& ent : selviews)
		{
			context().registry().remove<CompSelection>(ent);
		}

		_selectEntities.clear();
	}

	void GamePlayTileBattle::onSelectChange(const EntitySet& selectEntities)
	{
		std::set<entt::entity> intersetVec;
		std::set_intersection(_selectEntities.begin(), _selectEntities.end(),
								selectEntities.begin(), selectEntities.end(),
								std::inserter(intersetVec, intersetVec.begin()));

		for (auto& ent : selectEntities)
		{
			if (!intersetVec.contains(ent))
			{
				// not selected, select now
				context().registry().emplace_or_replace<CompSelection>(ent, CompSelection{});
			}
		}
		
		for (auto& ent : _selectEntities)
		{
			if (!intersetVec.contains(ent))
			{
				// not select now, unselect
				context().registry().remove<CompSelection>(ent);
			}
		}

		_selectEntities = selectEntities;
	}

	void GamePlayTileBattle::onSkipMove()
	{
		for (auto& entity : _selectEntities)
		{
			auto pGameTurn = context().registry().try_get<CompGameTurn>(entity);
			if (pGameTurn && _turnType == GameTurnType::Moving)
			{
				pGameTurn->running = false;
			}
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

	void GamePlayTileBattle::draw()
	{
		context().painter().drawRect(Color::Red, _selectRect, 0, 2);
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
				startAutoFightFlow(actor);
			}
		}
	}

	void GamePlayTileBattle::onActorCreate(entt::entity actor)
	{
		context().registry().emplace<CompGameTurn>(actor, CompGameTurn{ false });
	}
	
	void GamePlayTileBattle::onActorDestroy(entt::entity actor)
	{
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

	void GamePlayTileBattle::startAutoFightFlow(entt::entity actor)
	{
		context().dispatcher().trigger(EvtRoleAutoAttack{ actor });
	}
	
}