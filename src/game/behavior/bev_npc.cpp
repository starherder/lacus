
#include "bev_npc.h"
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


	bool BevNode_FindPatrolPos::load(const bevtree::XmlNode* node)
	{
		return true;
	}

	Status BevNode_FindPatrolPos::update()
	{
		if (!_context || _actor==entt::null) 
		{
			return Status::Failure;
		}

		auto& trans = _context->registry().get<CompTransform>(_actor);
		auto& motion = _context->registry().get<CompMotion>(_actor);
		const auto& src = trans.position;
	
		auto pPatrolCom = _context->registry().try_get<CompNpcPatrol>(_actor);
		if(pPatrolCom)
		{
			while(true) 
			{
				float angle = utility::random(-3.14f, 3.14f);
				float dist = utility::random(pPatrolCom->patrol_radius/2.0f, pPatrolCom->patrol_radius);

				Vec2 offset = Vec2{sin(angle), cos(angle)} * dist;
				Vec2 dest = pPatrolCom->origin_pos + offset;

				Vec2i srcGrid = _context->currentScene().getGridFromPos(src);
				Vec2i dstGrid = _context->currentScene().getGridFromPos(dest);

				auto path = _context->pathFinder().findPath(srcGrid, dstGrid);
				if(!path)
				{
					spdlog::warn("find patrol pos ({}, {}) unreachable, find again.", dest.x, dest.y);
					continue;
				}

				motion.path.swap(path.value());

				motion.targetGrid = dstGrid;

				//spdlog::info("BevNode_FindPatrolPos: path find success!  dest = ({}, {})", dest.x, dest.y);
				return Status::Success;
			}
		}

		return Status::Success;
	}

	void BevNode_FindPatrolPos::initialize()
	{
		_context = getBlackboard()->getValue<GameContext*>("context", nullptr);
		_actor = getBlackboard()->getValue<entt::entity>("actor", entt::null);

		//spdlog::info("BevNode_FindPatrolPos: initialize");
	}

	void BevNode_FindPatrolPos::terminate(Status s)
	{
		//spdlog::info("BevNode_FindPatrolPos: terminate");
	}

	/////////////////////////////////////////////////////////////////////////////////

	bool BevNode_PatrolMove::load(const XmlNode* node) 
	{
		return true;
	}

	Status BevNode_PatrolMove::update() 
	{
		if(_finished) 
		{
			//spdlog::info("BevNode_PatrolMove: update success");
			return Status::Success;
		}

		return Status::Running;
	}

	void BevNode_PatrolMove::initialize() 
	{
		_context = getBlackboard()->getValue<GameContext*>("context", nullptr);
		_actor = getBlackboard()->getValue<entt::entity>("actor", entt::null);
		if(!_context || _actor == entt::null) 
		{
			spdlog::error("actor NOT valid.");
			return;
		}

		_finished = false;

		auto& motion = _context->registry().get<CompMotion>(_actor);
		_context->dispatcher().trigger(MoveToGrid{ _actor, motion.targetGrid, false });

		_context->dispatcher().sink<MotionStop>().connect<&BevNode_PatrolMove::onMotionStop>(this);

		//spdlog::info("BevNode_PatrolMove: initialize");
	}
	
	void BevNode_PatrolMove::terminate(Status s) 
	{
		//spdlog::info("BevNode_PatrolMove: terminate");
	}

	void BevNode_PatrolMove::onMotionStop(const MotionStop& e)
	{
		if(e.actor != _actor) 
		{
			return;
		}

		//spdlog::info("BevNode_PatrolMove: onMotionStop");
		_finished = true;
	}

	//////////////////////////////////////////////////////////////////////

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
			//spdlog::info("BevNode_Idle: update success");
			return Status::Success;
		}

		return Status::Running;
	}

	void BevNode_Idle::initialize()
	{
		_context = getBlackboard()->getValue<GameContext*>("context", nullptr);
		_idleCurSeconds = 0.0f;

		//spdlog::info("BevNode_Idle: initialize");
	}

	void BevNode_Idle::terminate(Status s) 
	{
		//spdlog::info("BevNode_Idle: terminate");
	}


	////////////////////////////////////////////////////


	bool BevNode_PickItem::load(const XmlNode* node)
	{
		return true;
	}

	Status BevNode_PickItem::update()
	{
		if (_pickOK)
		{
			spdlog::info("BevNode_PickItem: finished.");
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
			spdlog::error("actor NOT valid.");
			return;
		}

		_pickOK = false;
		_context->dispatcher().sink< RolePickItem>().connect<&BevNode_PickItem::onRolePickItem>(this);
		_context->dispatcher().sink< RoleCrossGrid>().connect<&BevNode_PickItem::onRoleEnterGrid>(this);
	}

	void BevNode_PickItem::terminate(Status s)
	{
	}

	Status BevNode_PickItem::checkPickUp()
	{
		auto& trans = _context->registry().get<CompTransform>(_actor);
		auto& motion = _context->registry().get<CompMotion>(_actor);

		const auto& src = trans.position;
		auto srcGrid = _context->currentScene().getGridFromPos(src);
		auto gridCenter = _context->currentScene().getGridCenterPos(srcGrid);

		auto pbevCom = _context->registry().try_get<CompBehavior>(_actor);
		if (!pbevCom)
		{
			return Status::Failure;
		}

		Vec2i dstGrid{ -1,-1 };
		auto radius = pbevCom->vision_dis;

		auto& gridsmap = _context->currentScene().getGridsInCircle(gridCenter, radius);
		for (auto& [dis, grid] : gridsmap)
		{
			auto& objects = _context->currentScene().getObjectsInGrid(grid);
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
			spdlog::warn("find pickable item pos ({}, {}) unreachable, find again.", dstGrid.x, dstGrid.y);
			return Status::Failure;
		}

		motion.path.swap(path.value());
		motion.targetGrid = dstGrid;

		spdlog::info("move to item grid({}, {}).", dstGrid.x, dstGrid.y);

		_context->dispatcher().trigger(MoveToGrid{ _actor, motion.targetGrid, false });

		return Status::Running;
	}

	void BevNode_PickItem::onRolePickItem(const RolePickItem& e)
	{
		if (e.actor == _actor) 
		{
			spdlog::info("BevNode_PickItem: onRolePickItem.");
			_pickOK = true;
		}
	}

	void BevNode_PickItem::onRoleEnterGrid(const RoleCrossGrid& e)
	{
		if (e.actor == _actor)
		{
			spdlog::info("BevNode_PickItem: role grid change, need check.");
			_needCheck = true;
		}
	}
}