
#include "bev_patrol_npc.h"
#include "bevtree/bevtree.h"

#include "game/scene/game_scene.h"
#include "game/scene/game_context.h"
#include "game/logic_config.h"
#include "utility/random.h"
#include "game/ecs/comm_comp.h"
#include "game/ecs/comm_event.h"

#include <array>

namespace game {


	RegisterBehaviorNode("patrol_findpath", BevNode_FindPatrolPos);
	RegisterBehaviorNode("patrol_move", BevNode_PatrolMove);
	RegisterBehaviorNode("patrol_idle", BevNode_Idle);
	RegisterBehaviorNode("random_emotion", BevNode_RandomEmotion);
	RegisterBehaviorNode("pick_item", BevNode_PickItem);
	RegisterBehaviorNode("defend_attack", BevNode_DefendAttack);
	RegisterBehaviorNode("in_motion_turn", BevNode_InMotionTurn);
	RegisterBehaviorNode("in_fight_turn", BevNode_InFightTurn);
	RegisterBehaviorNode("hp_in_danger", BevNode_HpInDanger);
	RegisterBehaviorNode("step_move_to_fight", BevNode_StepMoveToFight);
	RegisterBehaviorNode("step_fight", BevNode_StepFight);
	RegisterBehaviorNode("step_move_to_flee", BevNode_StepMoveToFlee);


	bool BevNode_FindPatrolPos::load(const bevtree::XmlNode* node)
	{
		return true;
	}

	Status BevNode_FindPatrolPos::update()
	{
		if (!_context || _context->registry().valid(_actor)==false) 
		{
			return Status::Failure;
		}

		auto ptrans = _context->registry().try_get<CompTransform>(_actor);
		if(!ptrans)
		{
			LogError("actor {} Have NO transform.", (uint32_t)_actor);
			return Status::Failure;
		}
		
		auto pmotion = _context->registry().try_get<CompAutoMotion>(_actor);
		if(!pmotion)
		{
			_context->registry().emplace_or_replace<CompAutoMotion>(_actor);
			pmotion = _context->registry().try_get<CompAutoMotion>(_actor);
		}

		auto& trans = *ptrans;
		auto& motion = *pmotion;
		const auto& src = trans.position;
	
		auto pPatrolCom = _context->registry().try_get<CompNpcPatrol>(_actor);
		if(pPatrolCom)
		{
			int LoopCount = 0;
			while(true) 
			{
				if (LoopCount++ > 20)
				{
					LogWarn("BevNode_FindPatrolPos: loop time > {}, break;", LoopCount);
					break;
				}

				float angle = utility::random(-3.14f, 3.14f);
				float dist = utility::random(pPatrolCom->patrol_radius/2.0f, pPatrolCom->patrol_radius);

				Vec2 offset = Vec2{sin(angle), cos(angle)} * dist;
				Vec2 dest = pPatrolCom->origin_pos + offset;

				Vec2i srcGrid = _context->scene().getGridFromPos(src);
				Vec2i dstGrid = _context->scene().getGridFromPos(dest);

				auto path = _context->pathFinder().findPath(srcGrid, dstGrid);
				if(!path)
				{
					continue;
				}

				motion.path.swap(path.value());

				motion.targetGrid = dstGrid;

				//LogInfo("BevNode_FindPatrolPos: path find success!  dest = ({}, {})", dest.x, dest.y);
				return Status::Success;
			}
		}

		return Status::Success;
	}

	void BevNode_FindPatrolPos::initialize()
	{
		_context = getBlackboard()->getValue<GameContext*>("context", nullptr);
		_actor = getBlackboard()->getValue<entt::entity>("actor", entt::null);

		//LogInfo("BevNode_FindPatrolPos: initialize");
	}

	void BevNode_FindPatrolPos::terminate(Status s)
	{
		//LogInfo("BevNode_FindPatrolPos: terminate");
	}


	// ---------------------------------------------------------------------


	bool BevNode_PatrolMove::load(const XmlNode* node) 
	{
		return true;
	}

	Status BevNode_PatrolMove::update() 
	{
		if(_finished) 
		{
			return Status::Success;
		}

		return Status::Running;
	}

	void BevNode_PatrolMove::initialize() 
	{
		_context = getBlackboard()->getValue<GameContext*>("context", nullptr);
		_actor = getBlackboard()->getValue<entt::entity>("actor", entt::null);
		if(!_context || _context->registry().valid(_actor) == false) 
		{
			LogError("actor NOT valid.");
			return;
		}

		_finished = false;

		auto& motion = _context->registry().get<CompAutoMotion>(_actor);
		_context->dispatcher().trigger(EvtMoveToGrid{ _actor, motion.targetGrid, false });
		_context->dispatcher().sink<EvtMotionStop>().connect<&BevNode_PatrolMove::onMotionStop>(this);
	}
	
	void BevNode_PatrolMove::terminate(Status s) 
	{
	}

	void BevNode_PatrolMove::onMotionStop(const EvtMotionStop& e)
	{
		if(e.actor != _actor) 
		{
			return;
		}

		_finished = true;
	}


	// ---------------------------------------------------------------------

	bool BevNode_Idle::load(const XmlNode* node) 
	{
		_idleTotalSeconds = node->FloatAttribute("time");
		return true;
	}

	Status BevNode_Idle::update()
	{
		if(!_context) 
		{ 
			return Status::Failure;
		}

		_idleCurSeconds += _context->frameTicker().deltaSeconds();
		if(_idleCurSeconds > _idleTotalSeconds) 
		{
			return Status::Success;
		}

		return Status::Running;
	}

	void BevNode_Idle::initialize()
	{
		_context = getBlackboard()->getValue<GameContext*>("context", nullptr);
		_idleCurSeconds = 0.0f;
	}

	void BevNode_Idle::terminate(Status s) 
	{
	}


	// ---------------------------------------------------------------------

	bool BevNode_RandomEmotion::load(const XmlNode* node)
	{
		_minSeconds = node->FloatAttribute("min_time", _minSeconds);
		_maxSeconds = node->FloatAttribute("max_time", _maxSeconds);
		_minSeconds = std::max(_minSeconds, 0.0f);
		_maxSeconds = std::max(_maxSeconds, _minSeconds);

		return true;
	}

	Status BevNode_RandomEmotion::update()
	{
		if (!_context || _context->registry().valid(_actor) == false)
		{
			return Status::Failure;
		}

		if (_context->currentTicks() >= _nextTriggerTicks)
		{
			showRandomEmotion();
			resetWaitSeconds();
		}

		return Status::Running;
	}

	void BevNode_RandomEmotion::initialize()
	{
		_context = getBlackboard()->getValue<GameContext*>("context", nullptr);
		_actor = getBlackboard()->getValue<entt::entity>("actor", entt::null);
		resetWaitSeconds();
	}

	void BevNode_RandomEmotion::terminate(Status s)
	{
	}

	void BevNode_RandomEmotion::resetWaitSeconds()
	{
		float waitSeconds = utility::random(_minSeconds, _maxSeconds);
		_nextTriggerTicks = _context ? _context->currentTicks() + (int64_t)(waitSeconds * 1000.0f) : 0;
	}

	void BevNode_RandomEmotion::showRandomEmotion()
	{
		if (!_context || _context->registry().valid(_actor) == false)
		{
			return;
		}

		auto emotionNames = _context->logicConfig().getAllEmotionNames();
		if (emotionNames.empty())
		{
			return;
		}

		int emotionIndex = utility::random(0, (int)emotionNames.size() - 1);
		int textIndex = utility::random(1, 10);

		EvtShowBubble evt;
		evt.actor = _actor;
		evt.emotion = emotionNames[emotionIndex];
		evt.text = "mur_" + std::to_string(textIndex);
		_context->dispatcher().trigger(evt);
	}


	// ---------------------------------------------------------------------


	bool BevNode_PickItem::load(const XmlNode* node)
	{
		return true;
	}

	Status BevNode_PickItem::update()
	{
		if (_pickOK)
		{
			//LogInfo("BevNode_PickItem: finished.");
			return Status::Success;
		}

		if (_needCheck)
		{
			_needCheck = false;

			return checkPickUp();
		}

		return Status::Success;
	}

	void BevNode_PickItem::initialize()
	{
		_context = getBlackboard()->getValue<GameContext*>("context", nullptr);
		_actor = getBlackboard()->getValue<entt::entity>("actor", entt::null);
		if (!_context || _actor == entt::null)
		{
			LogError("actor NOT valid.");
			return;
		}

		_pickOK = false;
		_context->dispatcher().sink< EvtRolePickItemStart>().connect<&BevNode_PickItem::onRolePickItemStart>(this);
		_context->dispatcher().sink< EvtRoleCrossGrid>().connect<&BevNode_PickItem::onRoleEnterGrid>(this);
	}

	void BevNode_PickItem::terminate(Status s)
	{
	}

	Status BevNode_PickItem::checkPickUp()
	{
		if (!_context->registry().valid(_actor))
		{
			return Status::Failure;
		}

		auto ptrans = _context->registry().try_get<CompTransform>(_actor);
		auto pmotion = _context->registry().try_get<CompAutoMotion>(_actor);
		if(!ptrans || !pmotion)
		{
			return Status::Failure;
		}

		auto& trans = *ptrans;
		auto& motion = *pmotion;

		const auto& src = trans.position;
		auto srcGrid = _context->scene().getGridFromPos(src);
		auto gridCenter = _context->scene().getGridCenterPos(srcGrid);

		auto pbevCom = _context->registry().try_get<CompBehavior>(_actor);
		if (!pbevCom)
		{
			return Status::Failure;
		}

		Vec2i dstGrid{ -1,-1 };
		auto radius = pbevCom->vision_dis;

		auto objects = _context->scene().getObjectsInCircle(gridCenter, radius);
		for (auto& [dis, obj] : objects)
		{
			auto pickable = _context->registry().try_get<CompPickable>(obj);
			if (pickable && pickable->picked == false)
			{
				dstGrid = _context->scene().getObjectGrid(obj);
				break;
			}
		}

		if (dstGrid.x < 0 || dstGrid.y < 0)
		{
			return Status::Failure;
		}

		auto path = _context->pathFinder().findPath(srcGrid, dstGrid);
		if (!path)
		{
			LogWarn("find pickable item pos ({}, {}) unreachable, find again.", dstGrid.x, dstGrid.y);
			return Status::Failure;
		}

		motion.path.swap(path.value());
		motion.targetGrid = dstGrid;

		_context->dispatcher().trigger(EvtMoveToGrid{ _actor, motion.targetGrid, false });

		return Status::Running;
	}

	void BevNode_PickItem::onRolePickItemStart(const EvtRolePickItemStart& e)
	{
		if (e.actor == _actor) 
		{
			//LogInfo("BevNode_PickItem: onRolePickItemStart.");
			_pickOK = true;
		}
	}

	void BevNode_PickItem::onRoleEnterGrid(const EvtRoleCrossGrid& e)
	{
		if (e.actor == _actor)
		{
			//LogInfo("BevNode_PickItem: role grid change, need check.");
			_needCheck = true;
		}
	}

	// ---------------------------------------------------------------------


	bool BevNode_DefendAttack::load(const XmlNode* node)
	{
		return true;
	}

	Status BevNode_DefendAttack::update()
	{
		return checkEnemy();
	}

	void BevNode_DefendAttack::initialize()
	{
		_context = getBlackboard()->getValue<GameContext*>("context", nullptr);
		_actor = getBlackboard()->getValue<entt::entity>("actor", entt::null);
		if (!_context || _actor == entt::null)
		{
			LogError("actor NOT valid.");
			return;
		}
	}

	void BevNode_DefendAttack::terminate(Status s)
	{
	}
	
	Status BevNode_DefendAttack::checkEnemy()
	{
		auto& trans = _context->registry().get<CompTransform>(_actor);
		auto& rolePos = trans.position;
		if(std::isnan(rolePos.x) || std::isnan(rolePos.y))
		{
			LogError("rolepos ({}, {}) is nan!!", rolePos.x, rolePos.y);
			return Status::Failure;
		}

		auto pNpcComm = _context->registry().try_get<CompComm>(_actor);
		if (!pNpcComm)
		{
			LogError("role {} No CompComm found.", (uint32_t)_actor);
			return Status::Failure;
		}

		auto& skills = _context->registry().get<CompSkills>(_actor);
		for (auto& skill_id : skills.skills)
		{
			auto& compName = _context->registry().get<CompNameId>(skill_id);
			auto& compSkill = _context->registry().get<CompSkillComm>(skill_id);

			if (compSkill.type != SkillType::Invalid)
			{
				// 需要目标，寻找目标
				auto dis = compSkill.distance;
				auto& objects = _context->scene().getObjectsInCircle(rolePos, dis);
				for (auto& [dis, target] : objects) 
				{
					if (_context->registry().valid(target) == false || target == _actor) 
					{ 
						continue; 
					}

					auto pdead = _context->registry().try_get<CompDead>(target);
					if (pdead) 
					{ 
						continue; 
					}

					auto pCompComm = _context->registry().try_get<CompComm>(target);
					if (pCompComm && pCompComm->type == ObjectType::Npc && pCompComm->side != pNpcComm->side) 
					{
						_context->dispatcher().trigger(EvtMotionSwitchState{ _actor, MotionState::Paused });
						_context->dispatcher().trigger(EvtCastSkillToObject{ _actor, target, skill_id });
						return Status::Success;
					}
				}
			}
		}

		return Status::Failure;
	}

	namespace
	{
		entt::entity findNearestEnemyEntity(GameContext& context, entt::entity actor)
		{
			auto actorTrans = context.registry().try_get<CompTransform>(actor);
			auto actorComm = context.registry().try_get<CompComm>(actor);
			if (!actorTrans || !actorComm)
			{
				return entt::null;
			}

			float visionDis = 300.0f;
			auto behavior = context.registry().try_get<CompBehavior>(actor);
			if (behavior)
			{
				visionDis = behavior->vision_dis;
			}

			const auto& objects = context.scene().getObjectsInCircle(actorTrans->position, visionDis);
			for (auto& [dis, target] : objects)
			{
				if (!context.registry().valid(target) || target == actor)
				{
					continue;
				}

				if (context.registry().try_get<CompDead>(target))
				{
					continue;
				}

				auto targetComm = context.registry().try_get<CompComm>(target);
				if (targetComm && targetComm->type == ObjectType::Npc && targetComm->side != actorComm->side)
				{
					return target;
				}
			}

			return entt::null;
		}

		bool canStepToGrid(GameContext& context, entt::entity actor, const Vec2i& grid)
		{
			if (context.scene().getGridWalkType(grid) == (int)tilemap::WalkType::Collision)
			{
				return false;
			}

			auto entityInGrid = context.scene().getOneObjectInGrid(grid, ObjectType::Npc);
			return !context.registry().valid(entityInGrid) || entityInGrid == actor;
		}

		bool tryStepToDir(GameContext& context, entt::entity actor, const Vec2i& dir)
		{
			if (dir.x * dir.x + dir.y * dir.y != 1)
			{
				return false;
			}

			if (!context.registry().try_get<CompMoveCfg>(actor))
			{
				return false;
			}

			auto stepMotion = context.registry().try_get<CompStepMotion>(actor);
			if (stepMotion && stepMotion->state != MotionState::Resting)
			{
				return false;
			}

			auto nextGrid = context.scene().getObjectGrid(actor) + dir;
			if (!canStepToGrid(context, actor, nextGrid))
			{
				return false;
			}

			context.dispatcher().trigger(EvtStepMove{ actor, dir, 1 });
			return true;
		}

		Vec2i getDirToEnemy(GameContext& context, entt::entity actor, entt::entity enemy)
		{
			auto actorGrid = context.scene().getObjectGrid(actor);
			auto enemyGrid = context.scene().getObjectGrid(enemy);
			auto delta = enemyGrid - actorGrid;

			if (std::abs(delta.x) > std::abs(delta.y) && delta.x != 0)
			{
				return { delta.x > 0 ? 1 : -1, 0 };
			}

			if (delta.y != 0)
			{
				return { 0, delta.y > 0 ? 1 : -1 };
			}

			if (delta.x != 0)
			{
				return { delta.x > 0 ? 1 : -1, 0 };
			}

			return { 0, 0 };
		}

		Vec2i getDirFromEnemy(GameContext& context, entt::entity actor, entt::entity enemy)
		{
			auto actorGrid = context.scene().getObjectGrid(actor);
			auto enemyGrid = context.scene().getObjectGrid(enemy);
			std::array<Vec2i, 4> dirs = {
				Vec2i{ 1, 0 },
				Vec2i{ -1, 0 },
				Vec2i{ 0, 1 },
				Vec2i{ 0, -1 },
			};

			Vec2i bestDir{ 0, 0 };
			int bestScore = std::abs(actorGrid.x - enemyGrid.x) + std::abs(actorGrid.y - enemyGrid.y);
			for (auto dir : dirs)
			{
				auto nextGrid = actorGrid + dir;
				if (!canStepToGrid(context, actor, nextGrid))
				{
					continue;
				}

				int score = std::abs(nextGrid.x - enemyGrid.x) + std::abs(nextGrid.y - enemyGrid.y);
				if (score > bestScore)
				{
					bestScore = score;
					bestDir = dir;
				}
			}

			return bestDir;
		}
	}

	bool BevNode_InMotionTurn::load(const XmlNode* node)
	{
		return true;
	}

	Status BevNode_InMotionTurn::update()
	{
		return _context && _context->gamePlay().isMoveStage() ? Status::Success : Status::Failure;
	}

	void BevNode_InMotionTurn::initialize()
	{
		_context = getBlackboard()->getValue<GameContext*>("context", nullptr);
	}

	void BevNode_InMotionTurn::terminate(Status s)
	{
	}

	bool BevNode_InFightTurn::load(const XmlNode* node)
	{
		return true;
	}

	Status BevNode_InFightTurn::update()
	{
		return _context && _context->gamePlay().isFightStage() ? Status::Success : Status::Failure;
	}

	void BevNode_InFightTurn::initialize()
	{
		_context = getBlackboard()->getValue<GameContext*>("context", nullptr);
	}

	void BevNode_InFightTurn::terminate(Status s)
	{
	}

	bool BevNode_HpInDanger::load(const XmlNode* node)
	{
		_hp = node->FloatAttribute("hp", _hp);
		return true;
	}

	Status BevNode_HpInDanger::update()
	{
		if (!_context || !_context->gamePlay().isMoveStage() || !_context->registry().valid(_actor))
		{
			return Status::Failure;
		}

		auto fightProp = _context->registry().try_get<CompFightProp>(_actor);
		return fightProp && fightProp->hp < _hp ? Status::Success : Status::Failure;
	}

	void BevNode_HpInDanger::initialize()
	{
		_context = getBlackboard()->getValue<GameContext*>("context", nullptr);
		_actor = getBlackboard()->getValue<entt::entity>("actor", entt::null);
	}

	void BevNode_HpInDanger::terminate(Status s)
	{
	}

	bool BevNode_StepMoveToFight::load(const XmlNode* node)
	{
		return true;
	}

	Status BevNode_StepMoveToFight::update()
	{
		if (!_context || !_context->registry().valid(_actor))
		{
			return Status::Failure;
		}

		if (!_context->gamePlay().isMoveStage())
		{
			return Status::Failure;
		}

		auto turn = _context->registry().try_get<CompGameTurn>(_actor);
		if (!turn || !turn->running)
		{
			return Status::Success;
		}

		if (_finished)
		{
			return Status::Running;
		}

		auto enemy = findNearestEnemy();
		if (enemy == entt::null || !tryStepToEnemy(enemy))
		{
			_finished = true;
			return Status::Running;
		}

		_finished = true;
		return Status::Running;
	}

	void BevNode_StepMoveToFight::initialize()
	{
		_context = getBlackboard()->getValue<GameContext*>("context", nullptr);
		_actor = getBlackboard()->getValue<entt::entity>("actor", entt::null);
		_finished = false;
		if (!_context || _actor == entt::null)
		{
			LogError("actor NOT valid.");
		}
	}

	void BevNode_StepMoveToFight::terminate(Status s)
	{
	}

	entt::entity BevNode_StepMoveToFight::findNearestEnemy()
	{
		return findNearestEnemyEntity(*_context, _actor);
	}

	bool BevNode_StepMoveToFight::tryStepToEnemy(entt::entity enemy)
	{
		auto dir = getDirToEnemy(*_context, _actor, enemy);
		return tryStepToDir(*_context, _actor, dir);
	}

	bool BevNode_StepFight::load(const XmlNode* node)
	{
		return true;
	}

	Status BevNode_StepFight::update()
	{
		if (!_context || !_context->registry().valid(_actor))
		{
			return Status::Failure;
		}

		if (!_context->gamePlay().isFightStage())
		{
			return Status::Failure;
		}

		auto turn = _context->registry().try_get<CompGameTurn>(_actor);
		if (!turn || !turn->running)
		{
			return Status::Success;
		}

		if (_finished)
		{
			return Status::Running;
		}

		_context->dispatcher().trigger(EvtRoleAutoAttack{ _actor });
		finishFightTurn();
		_finished = true;
		return Status::Success;
	}

	void BevNode_StepFight::initialize()
	{
		_context = getBlackboard()->getValue<GameContext*>("context", nullptr);
		_actor = getBlackboard()->getValue<entt::entity>("actor", entt::null);
		_finished = false;
		if (!_context || _actor == entt::null)
		{
			LogError("actor NOT valid.");
		}
	}

	void BevNode_StepFight::terminate(Status s)
	{
	}

	void BevNode_StepFight::finishFightTurn()
	{
		auto turn = _context ? _context->registry().try_get<CompGameTurn>(_actor) : nullptr;
		if (turn)
		{
			turn->running = false;
		}
	}

	bool BevNode_StepMoveToFlee::load(const XmlNode* node)
	{
		return true;
	}

	Status BevNode_StepMoveToFlee::update()
	{
		if (!_context || !_context->registry().valid(_actor))
		{
			return Status::Failure;
		}

		if (!_context->gamePlay().isMoveStage())
		{
			return Status::Failure;
		}

		auto turn = _context->registry().try_get<CompGameTurn>(_actor);
		if (!turn || !turn->running)
		{
			return Status::Success;
		}

		if (_finished)
		{
			return Status::Running;
		}

		auto enemy = findNearestEnemy();
		if (enemy == entt::null || !tryStepFromEnemy(enemy))
		{
			_finished = true;
			return Status::Running;
		}

		_finished = true;
		return Status::Running;
	}

	void BevNode_StepMoveToFlee::initialize()
	{
		_context = getBlackboard()->getValue<GameContext*>("context", nullptr);
		_actor = getBlackboard()->getValue<entt::entity>("actor", entt::null);
		_finished = false;
		if (!_context || _actor == entt::null)
		{
			LogError("actor NOT valid.");
		}
	}

	void BevNode_StepMoveToFlee::terminate(Status s)
	{
	}

	entt::entity BevNode_StepMoveToFlee::findNearestEnemy()
	{
		return findNearestEnemyEntity(*_context, _actor);
	}

	bool BevNode_StepMoveToFlee::tryStepFromEnemy(entt::entity enemy)
	{
		auto dir = getDirFromEnemy(*_context, _actor, enemy);
		return tryStepToDir(*_context, _actor, dir);
	}
}