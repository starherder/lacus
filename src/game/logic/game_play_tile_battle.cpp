#include "game/logic/game_play_tile_battle.h"
#include "game/ecs/comm_event.h"
#include "game/scene/game_scene.h"
#include "ui/gui_manager.h"
#include "magic_enum/magic_enum.h"
#include <algorithm>
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

		auto selent = context().scene().findObjectAtPos(_selectRect.pos());
		if (canSelectEntity(selent))
		{
			onSelectChange({ selent });
		}
		else
		{
			unselectAll();
		}
	}

	void GamePlayTileBattle::onMouseLeftRelease(const Vec2& pos)
	{
		_selectRect = { 0,0,0,0 };
	}
	
	void GamePlayTileBattle::onMouseLeftDrag(const Vec2& pos, const Vec2& offset)
	{
		if (ui::GuiManager::inst().isDragging())
		{
			_selectRect = { 0,0,0,0 };
			return;
		}

		auto scenePos = context().camera().screenToWorld(pos);
		_selectRect.w = scenePos.x - _selectRect.x;
		_selectRect.h = scenePos.y - _selectRect.y;

		EntitySet selents;
		for (auto& entity : context().scene().getObjectsInRect(_selectRect))
		{
			if (canSelectEntity(entity))
			{
				selents.insert(entity);
			}
		}
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

		enum class MoveCheckResult
		{
			Clear,
			Pending,
			Blocked,
		};

		bool mvHorizonal = (dir.x != 0);
		bool mvForward = (dir.x + dir.y) > 0;
		std::map<int, std::map<int, entt::entity>> sortEntitiyLines;
		EntitySet pendingEntities;

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

			pendingEntities.insert(entity);
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

		auto checkEnterGrid = [&](entt::entity entity, const Vec2i& grid)
		{
			if (context().scene().getGridWalkType(grid) == (int)tilemap::WalkType::Collision)
			{
				return MoveCheckResult::Blocked;
			}

			MoveCheckResult result = MoveCheckResult::Clear;
			for (auto& obj : context().scene().getObjectsInGrid(grid))
			{
				if (obj == entity)
				{
					continue;
				}

				auto pComm = context().registry().try_get<CompComm>(obj);
				if (!pComm || pComm->type != ObjectType::Npc)
				{
					continue;
				}

				if (movableEntities.contains(obj))
				{
					continue;
				}

				if (pendingEntities.contains(obj))
				{
					result = MoveCheckResult::Pending;
					continue;
				}

				return MoveCheckResult::Blocked;
			}

			return result;
		};

		auto checkEntity = [&](entt::entity entity)
		{
			auto transform = context().registry().try_get<CompTransform>(entity);
			if (!transform)
			{
				return MoveCheckResult::Blocked;
			}

			MoveCheckResult result = MoveCheckResult::Clear;
			auto enterGrids = context().scene().getObjectMoveEnterGrids(*transform, dir);
			for (auto& grid : enterGrids)
			{
				auto gridResult = checkEnterGrid(entity, grid);
				if (gridResult == MoveCheckResult::Blocked)
				{
					return MoveCheckResult::Blocked;
				}

				if (gridResult == MoveCheckResult::Pending)
				{
					result = MoveCheckResult::Pending;
				}
			}

			return result;
		};

		auto tryResolveEntity = [&](entt::entity entity)
		{
			if (!pendingEntities.contains(entity))
			{
				return false;
			}

			if (checkEntity(entity) != MoveCheckResult::Clear)
			{
				return false;
			}

			movableEntities.insert(entity);
			pendingEntities.erase(entity);
			return true;
		};

		bool progress = true;
		while (progress)
		{
			progress = false;
			for (auto& [c1, others] : sortEntitiyLines)
			{
				if (mvForward)
				{
					for (auto it = others.rbegin(); it != others.rend(); ++it)
					{
						progress = tryResolveEntity(it->second) || progress;
					}
				}
				else
				{
					for (auto& [c2, entity] : others)
					{
						progress = tryResolveEntity(entity) || progress;
					}
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

	bool GamePlayTileBattle::canSelectEntity(entt::entity entity)
	{
		if (!context().registry().valid(entity))
		{
			return false;
		}

		auto comm = context().registry().try_get<CompComm>(entity);
		return comm && comm->type == ObjectType::Npc;
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
		if (ui::GuiManager::inst().isDragging() || (_selectRect.w == 0.0f && _selectRect.h == 0.0f))
		{
			return;
		}

		auto rect = context().camera().projectRect(_selectRect);
		context().painter().drawRect(Color::Red, rect, 0, 2);
	}

	bool GamePlayTileBattle::hasMovingSelectedEntity()
	{
		for (auto& entity : _selectEntities)
		{
			if (!context().registry().valid(entity))
			{
				continue;
			}

			auto motion = context().registry().try_get<CompStepMotion>(entity);
			if (motion && motion->state == MotionState::Moving)
			{
				return true;
			}
		}

		return false;
	}

	std::optional<Vec2> GamePlayTileBattle::getSelectedEntitiesCenter()
	{
		Vec2 center = { 0.0f, 0.0f };
		int count = 0;
		for (auto& entity : _selectEntities)
		{
			if (!context().registry().valid(entity))
			{
				continue;
			}

			auto transform = context().registry().try_get<CompTransform>(entity);
			if (!transform)
			{
				continue;
			}

			center += transform->position;
			++count;
		}

		if (count == 0)
		{
			return std::nullopt;
		}

		return center / (float)count;
	}

	Vec2 GamePlayTileBattle::clampCameraPos(const Vec2& pos)
	{
		auto sceneSize = context().scene().sceneSize();
		auto viewSize = context().camera().getSize();
		return {
			std::clamp(pos.x, 0.0f, std::max(0.0f, sceneSize.x - viewSize.x)),
			std::clamp(pos.y, 0.0f, std::max(0.0f, sceneSize.y - viewSize.y))
		};
	}

	void GamePlayTileBattle::followMovingSelection()
	{
		if (!hasMovingSelectedEntity())
		{
			return;
		}

		auto center = getSelectedEntitiesCenter();
		if (!center)
		{
			return;
		}

		auto target = center.value() - context().camera().getSize() / 2.0f;
		context().camera().setPos(clampCameraPos(target));
	}

	void GamePlayTileBattle::update(float deltaTime)
	{
		followMovingSelection();

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
				auto behavior = context().registry().try_get<CompBehavior>(actor);
				if (!behavior || !behavior->bevtree || behavior->bevtree->name() != "npc_step_battle")
				{
					startAutoFightFlow(actor);
				}
			}
		}
	}

	void GamePlayTileBattle::onActorCreate(entt::entity actor)
	{
		auto compComm = context().registry().try_get<CompComm>(actor);
		if(compComm && compComm->type == ObjectType::Npc)
		{
			context().registry().emplace<CompGameTurn>(actor, CompGameTurn{ _turnType == GameTurnType::Moving });
		}
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
		if (_turnType == GameTurnType::Moving)
		{
			auto behavior = context().registry().try_get<CompBehavior>(actor);
			if (behavior && behavior->bevtree && behavior->bevtree->name() == "npc_step_battle")
			{
				return;
			}
		}

		auto& turn = context().registry().get<CompGameTurn>(actor);
		turn.running = false;
	}

	void GamePlayTileBattle::startAutoFightFlow(entt::entity actor)
	{
		context().dispatcher().trigger(EvtRoleAutoAttack{ actor });
	}
	
}