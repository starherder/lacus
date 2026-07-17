#include "system_dead.h"

#include "game/game_config.h"


namespace game 
{
	DeclareEcsSystem(DeadSystem, EcsPriority::Middle);


	DeadSystem::DeadSystem(GameContext& context) : EcsSystem(context)
	{
		_context.dispatcher().sink<EvtRoleDead>().connect<&DeadSystem::onRoleDead>(this);
	}

	DeadSystem::~DeadSystem()
	{
	}

	void DeadSystem::update(float delta)
	{
		std::vector<entt::entity> pending;
		auto destroyViews = _context.registry().view<CompDestroy>();
		for (auto& ent : destroyViews)
		{
			pending.push_back(ent);
			_context.dispatcher().trigger(EvtRoleDestroyed{ent});
		}

		// destroy
		for (auto& ent : pending)
		{
			_context.registry().destroy(ent);
		}

		// ------------------------------------------------------------------------

		auto deadViews = _context.registry().view<CompDead>();
		for (auto& ent : deadViews)
		{
			auto& compDead = deadViews.get<CompDead>(ent);
			if (compDead.ticks <= 0)
			{
				auto roleEvent = _context.registry().try_get<CompRoleEvent>(ent);
				_context.dispatcher().trigger(EvtRoleDead{ ent, compDead.killer, roleEvent ? roleEvent->dead : "" });
			}
			else if (compDead.ticks > _context.gameConfig().dying_ticks)
			{
				_context.scene().destroyObject(ent);
			}
			compDead.ticks += _context.applicaton().frameTicker().deltaTicks();
		}
	}

	void DeadSystem::onRoleDead(const EvtRoleDead& e)
	{
		dropAllItems(e.actor);
	}

	void DeadSystem::dropAllItems(entt::entity actor)
	{
		auto pTransComp = _context.registry().try_get<CompTransform>(actor);
		auto pItemComp = _context.registry().try_get<CompItems>(actor);
		if (!pTransComp || ! pItemComp)
		{
			return;
		}

		auto drop_range = _context.gameConfig().pickup.dead_drop_range;

		auto& item_list = pItemComp->items;

		for (auto& item : item_list)
		{
			auto& nameId = _context.registry().get<CompNameId>(item);
			//LogInfo("item.cfg = {}", nameId.cfg_id);

			auto pItemTrans = _context.registry().try_get<CompTransform>(item);
			auto pItemDisplay = _context.registry().try_get<CompDisplay>(item);
			if (!pItemTrans || !pItemDisplay) 
			{
				continue;
			}

			Vec2 dest_pos = pTransComp->position;

			int loop_check = 0;
			while (true)
			{
				if (loop_check++ > 20) break;

				float angle = utility::random(-3.14f, 3.14f);
				float dist = utility::random(drop_range / 2.0f, (float)drop_range);

				Vec2 offset = Vec2{ sin(angle), cos(angle) } *dist;
				Vec2 dest = pTransComp->position + offset;
				Vec2i dstGrid = _context.scene().getGridFromPos(dest);

				if (_context.scene().getGridWalkType(dstGrid) != (int)tilemap::WalkType::Collision)
				{
					dest_pos = dest;
					break;
				}
			}

			pItemDisplay->visible = true;

			//pItemTrans->position = dest_pos;
			_context.scene().setObjectPos(item, dest_pos);
			_context.scene().addObjectToScene(item);
		}
	}

}