#include "system_bevtree.h"


namespace game 
{

	void BevTreeSystem::update(float delta) 
	{
		auto ent_view = _context.registry().view<CompBevtree>();
		for (auto& ent : ent_view)
		{
			auto& bevcom = ent_view.get<CompBevtree>(ent);
			if (bevcom.bevtree) 
			{
				bevcom.bevtree->update();
			}
		}
	}
}

