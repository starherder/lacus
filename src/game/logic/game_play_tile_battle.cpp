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
		context.eventDispatcher().onMouseMotion.connect(this, &GamePlayTileBattle::onMouseMotion, -1);
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
		
		checkMoveBlock(dir);
		
		for(auto& entity : _selectEntities)
		{
			if (context().registry().valid(entity) == false ||
				context().registry().try_get<CompMoveCfg>(entity) == nullptr)
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

	void GamePlayTileBattle::checkMoveBlock(const Vec2i& dir)
	{
		// {0,0}，{1,1}，{-1,1}，{1，-1}都排除
		if (std::abs(dir.x) == std::abs(dir.y))
		{
			return;
		}
		
		bool mvHorizonal =  (dir.x != 0);
		bool mvForward = (dir.x + dir.y) > 0;
		
		std::map<int, std::map<int, entt::entity>> sortEntitiyLines;

		GridEntityMap allEntityGrids;

		// 先把移动方向上的每一行角色按照前后位置排序
		for (auto& entity : _selectEntities)
		{
			if (!context().registry().valid(entity))
			{
				continue;
			}

			auto& transform = context().registry().get<CompTransform>(entity);
			auto grid = context().scene().getGridFromPos(transform.position);
			allEntityGrids.insert({ grid, entity });

			if (mvHorizonal)
			{
				sortEntitiyLines[grid.y].insert({grid.x, entity});
			}else
			{
				sortEntitiyLines[grid.x].insert({grid.y, entity});
			}
		}

		// 按运动方向，从前到后检查每一个对象
		for (auto& [c1, others] : sortEntitiyLines)
		{
			if (mvForward)
			{
				for (auto it=others.rbegin(); it!=others.rend(); ++it)
				{
					auto c2 = it->first;
					auto entity = it->second;
					
					checkLineBlock({c2, c1}, entity, dir);
				}
			}
			else
			{
				for (auto& [c2, entity] : others)
				{
					checkLineBlock({c1, c2}, entity, dir);
				}
			}
		}
	}
	
	// 检查每一个对象，如果它前面有障碍物，或者有被阻挡助的角色，则他也被阻挡
	void GamePlayTileBattle::checkLineBlock(Vec2i grid, entt::entity entity, Vec2i dir)
	{
		Vec2i preGrid = grid + dir;

		// 前面有障碍物，阻挡
		auto walkType = context().scene().getGridWalkType(preGrid);
		if (walkType == (int)tilemap::WalkType::Collision)
		{
			context().registry().emplace_or_replace<CompMoveBlocked>(entity);
			return;
		}

		// 前面有没有其他人
		auto preEntity = context().scene().getOneObjectInGrid(preGrid, ObjectType::Npc);
		if (!context().registry().valid(preEntity))
		{
			return;
		}

		// 没有comm控件，不知道什么东西，可以走
		auto commMe = context().registry().try_get<CompComm>(entity);
		auto commPre = context().registry().try_get<CompComm>(preEntity);
		if (!commMe || !commPre)
		{
			return;
		}

		// 跟我不是一边的，阻挡
		if (commMe->side != commPre->side)
		{
			context().registry().emplace_or_replace<CompMoveBlocked>(entity);
		}

		// 前面那个人也被阻挡了
		auto blocked = context().registry().try_get<CompMoveBlocked>(preEntity);
		if (blocked)
		{
			context().registry().emplace_or_replace<CompMoveBlocked>(entity);
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