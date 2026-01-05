#include "system_dead.h"



namespace game 
{


	void DeadSystem::update(float delta)
	{
		std::vector<CompNameId> pending;

		auto views = _context.registry().view<CompNameId, CompComm>();
		for (auto& ent : views) 
		{
			auto& nameComp = _context.registry().get<CompNameId>(ent);
			auto& commComp = _context.registry().get<CompComm>(ent);

			if (commComp.state == LifeState::Destroy)
			{
				pending.push_back(nameComp);
			}
		}

		// destroy
		for (auto& nameComp : pending)
		{
			spdlog::info("object: id {}, name {}, cfg {} destroy !", (int)nameComp.id, nameComp.name, nameComp.cfg_id);
			_context.registry().destroy(nameComp.id);
		}
	}








}