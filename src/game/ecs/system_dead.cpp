#include "system_dead.h"



namespace game 
{


	void DeadSystem::update(float delta)
	{
		std::vector<entt::entity> pending;

		auto deadViews = _context.registry().view<CompDead>();
		for (auto& ent : deadViews)
		{
			//auto& nameComp = views.get<CompNameId>(ent);
		}

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
	}








}