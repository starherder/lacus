#include "system_dead.h"



namespace game 
{


	void DeadSystem::update(float delta)
	{
		std::vector<CompNameId> pending;

		auto deadViews = _context.registry().view<CompNameId, CompDead>();
		for (auto& ent : deadViews)
		{
			//auto& nameComp = views.get<CompNameId>(ent);
		}

		auto destroyViews = _context.registry().view<CompNameId, CompDestroy>();
		for (auto& ent : destroyViews)
		{
			auto& nameComp = destroyViews.get<CompNameId>(ent);
			pending.push_back(nameComp);
		}

		// destroy
		for (auto& nameComp : pending)
		{
			spdlog::info("object: id {}, name {}, cfg {} destroy !", (int)nameComp.id, nameComp.name, nameComp.cfg_id);
			_context.registry().destroy(nameComp.id);
		}
	}








}