#include "system_dead.h"



namespace game 
{


	void DeadSystem::update(float delta)
	{
		std::vector<entt::entity> pending;
		auto destroyViews = _context.registry().view<CompDestroy>();
		for (auto& ent : destroyViews)
		{
			pending.push_back(ent);
		}

		// destroy
		for (auto& ent : pending)
		{
			spdlog::info("object: id {} destroy !", (uint32_t)ent);
			_context.registry().destroy(ent);
		}

		// ------------------------------------------------------------------------

		auto deadViews = _context.registry().view<CompDead>();
		for (auto& ent : deadViews)
		{
			auto& compDead = deadViews.get<CompDead>(ent);
			if (compDead.ticks <= 0)
			{
				_context.dispatcher().trigger(RoleDead{ent});
			}
			else if (compDead.ticks > _context.gameConfig().dying_ticks)
			{
				_context.registry().emplace_or_replace<CompDestroy>(ent);
			}
			compDead.ticks += _context.applicaton().frameTicker().deltaTicks();
		}
	}








}