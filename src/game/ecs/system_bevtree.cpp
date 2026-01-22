#include "system_bevtree.h"
#include "comp_fight.h"

namespace game 
{
	DeclareEcsSystem(BevTreeSystem, EcsPriority::Middle);

	void BevTreeSystem::update(float delta) 
	{
		auto ent_view = _context.registry().view<CompBehavior>();
		for (auto& ent : ent_view)
		{
			auto pdead = _context.registry().try_get<CompDead>(ent);
			if(pdead)
			{
				continue;
			}

			auto& bevcom = ent_view.get<CompBehavior>(ent);
			if (bevcom.bevtree) 
			{
				bevcom.bevtree->update();
			}
		}
	}
}

