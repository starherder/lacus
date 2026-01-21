
#include "bev_patrol_npc.h"
#include "bevtree/bevtree.h"

#include "game/scene/game_scene.h"
#include "game/scene/game_context.h"
#include "utility/random.h"
#include "game/ecs/comm_comp.h"
#include "game/ecs/comm_event.h"


namespace game {


	RegisterBehaviorNode("patrol_findpath", BevNode_FindPatrolPos);
	RegisterBehaviorNode("patrol_move", BevNode_PatrolMove);
	RegisterBehaviorNode("patrol_idle", BevNode_Idle);
	RegisterBehaviorNode("pick_item", BevNode_PickItem);
	RegisterBehaviorNode("defend_attack", BevNode_DefendAttack);


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
		auto pmotion = _context->registry().try_get<CompMotion>(_actor);
		if(!ptrans || !pmotion)
		{
			SPDLOG_ERROR("actor {} Have NO transform nor motion.", (uint32_t)_actor);
			return Status::Failure;
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
					SPDLOG_WARN("BevNode_FindPatrolPos: loop time > {}, break;", LoopCount);
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

				//SPDLOG_INFO("BevNode_FindPatrolPos: path find success!  dest = ({}, {})", dest.x, dest.y);
				return Status::Success;
			}
		}

		return Status::Success;
	}

	void BevNode_FindPatrolPos::initialize()
	{
		_context = getBlackboard()->getValue<GameContext*>("context", nullptr);
		_actor = getBlackboard()->getValue<entt::entity>("actor", entt::null);

		//SPDLOG_INFO("BevNode_FindPatrolPos: initialize");
	}

	void BevNode_FindPatrolPos::terminate(Status s)
	{
		//SPDLOG_INFO("BevNode_FindPatrolPos: terminate");
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
			SPDLOG_ERROR("actor NOT valid.");
			return;
		}

		_finished = false;

		auto& motion = _context->registry().get<CompMotion>(_actor);
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


	bool BevNode_PickItem::load(const XmlNode* node)
	{
		return true;
	}

	Status BevNode_PickItem::update()
	{
		if (_pickOK)
		{
			SPDLOG_INFO("BevNode_PickItem: finished.");
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
			SPDLOG_ERROR("actor NOT valid.");
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
		auto pmotion = _context->registry().try_get<CompMotion>(_actor);
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

		auto& gridsmap = _context->scene().getGridsInCircle(gridCenter, radius);
		for (auto& [dis, grid] : gridsmap)
		{
			auto& objects = _context->scene().getObjectsInGrid(grid);
			for (auto& obj : objects)
			{
				auto pickable = _context->registry().try_get<CompPickable>(obj);
				if (pickable && pickable->picked == false)
				{
					dstGrid = grid;
					break;
				}
			}
		}

		if (dstGrid.x < 0 || dstGrid.y < 0)
		{
			return Status::Failure;
		}

		auto path = _context->pathFinder().findPath(srcGrid, dstGrid);
		if (!path)
		{
			SPDLOG_WARN("find pickable item pos ({}, {}) unreachable, find again.", dstGrid.x, dstGrid.y);
			return Status::Failure;
		}

		motion.path.swap(path.value());
		motion.targetGrid = dstGrid;

		SPDLOG_INFO("move to item grid({}, {}).", dstGrid.x, dstGrid.y);

		_context->dispatcher().trigger(EvtMoveToGrid{ _actor, motion.targetGrid, false });

		return Status::Running;
	}

	void BevNode_PickItem::onRolePickItemStart(const EvtRolePickItemStart& e)
	{
		if (e.actor == _actor) 
		{
			SPDLOG_INFO("BevNode_PickItem: onRolePickItemStart.");
			_pickOK = true;
		}
	}

	void BevNode_PickItem::onRoleEnterGrid(const EvtRoleCrossGrid& e)
	{
		if (e.actor == _actor)
		{
			//SPDLOG_INFO("BevNode_PickItem: role grid change, need check.");
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
			SPDLOG_ERROR("actor NOT valid.");
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
			SPDLOG_ERROR("rolepos ({}, {}) is nan!!", rolePos.x, rolePos.y);
			return Status::Failure;
		}

		auto pNpcComm = _context->registry().try_get<CompComm>(_actor);
		if (!pNpcComm)
		{
			SPDLOG_ERROR("role {} No CompComm found.", (uint32_t)_actor);
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
						_context->dispatcher().trigger(EvtCastSkillToObject{ _actor, target, skill_id });
						return Status::Success;
					}
				}
			}
		}

		return Status::Failure;
	}
}