#include "game/logic/game_play_tile_battle.h"
#include "game/ecs/comm_event.h"
#include "game/scene/game_scene.h"
#include "magic_enum/magic_enum.h"
#include <iterator>

namespace game
{
	GamePlayTileBattle::GamePlayTileBattle(GameContext& context) : GamePlay(context)
	{
		context.eventDispatcher().onMouseLeftDown.connect(this, &GamePlayTileBattle::onMouseLeftPressed, -1);
		context.eventDispatcher().onMouseLeftUp.connect(this, &GamePlayTileBattle::onMouseLeftRelease, -1);
		context.eventDispatcher().onMouseLeftDrag.connect(this, &GamePlayTileBattle::onMouseLeftDrag, -1);
		context.eventDispatcher().onMouseMotion.connect(this, &GamePlayTileBattle::onMouseMotion, -1);

		context.eventDispatcher().onKeyDown.connect(this, &GamePlayTileBattle::onKeyDown, -1);
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

	void GamePlayTileBattle::onMouseMotion(const Vec2& pos, const Vec2& offset )
	{
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
		
		auto movableEntities = getMovableSelectedEntities(dir);
		if (movableEntities.empty())
		{
			return;
		}
		
		for(auto& entity : _selectEntities)
		{
			if (!movableEntities.contains(entity))
			{
				continue;
			}

			uint8_t moveGrids = 1;
			context().dispatcher().trigger(EvtStepMove{ entity, dir, moveGrids });
		}
	}

	GamePlayTileBattle::EntitySet GamePlayTileBattle::getMovableSelectedEntities(const Vec2i& dir)
	{
		EntitySet movableEntities;

		// dir 只支持 4 个方向
		if (dir.x * dir.x + dir.y * dir.y != 1)
		{
			return movableEntities;
		}

		bool mvHorizonal = (dir.x != 0);
		bool mvForward = (dir.x + dir.y) > 0;
		std::map<int, std::map<int, entt::entity>> sortEntitiyLines;

		for (auto& entity : _selectEntities)
		{
			if (!context().registry().valid(entity) ||
				context().registry().try_get<CompMoveCfg>(entity) == nullptr)
			{
				continue;
			}

			auto transform = context().registry().try_get<CompTransform>(entity);
			if (transform == nullptr)
			{
				continue;
			}

			auto grid = context().scene().getGridFromPos(transform->position);
			if (mvHorizonal)
			{
				sortEntitiyLines[grid.y].insert({ grid.x, entity });
			}
			else
			{
				sortEntitiyLines[grid.x].insert({ grid.y, entity });
			}
		}

		for (auto& [c1, others] : sortEntitiyLines)
		{
			auto checkEntity = [&](int c2, entt::entity entity)
			{
				Vec2i grid = mvHorizonal ? Vec2i{ c2, c1 } : Vec2i{ c1, c2 };
				Vec2i nextGrid = grid + dir;

				if (context().scene().getGridWalkType(nextGrid) == (int)tilemap::WalkType::Collision)
				{
					return;
				}

				auto nextEntity = context().scene().getOneObjectInGrid(nextGrid, ObjectType::Npc);
				if (!context().registry().valid(nextEntity) || movableEntities.contains(nextEntity))
				{
					movableEntities.insert(entity);
				}
			};

			if (mvForward)
			{
				for (auto it = others.rbegin(); it != others.rend(); ++it)
				{
					checkEntity(it->first, it->second);
				}
			}
			else
			{
				for (auto& [c2, entity] : others)
				{
					checkEntity(c2, entity);
				}
			}
		}

		return movableEntities;
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
	
	entt::entity GamePlayTileBattle::getSelectEntity() 
	{
		if(_selectEntities.empty())
		{
			return entt::null;
		}

		return *(_selectEntities.begin());
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
		auto turnViews = context().registry().view<CompGameTurn>();
		for(auto& ent : turnViews)
		{
			auto& compTurn = context().registry().get<CompGameTurn>(ent);
			compTurn.running = false;
		}

#if 0
		for (auto& entity : _selectEntities)
		{
			auto pGameTurn = context().registry().try_get<CompGameTurn>(entity);
			if (pGameTurn && _turnType == GameTurnType::Moving)
			{
				pGameTurn->running = false;
			}
		}
#endif
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