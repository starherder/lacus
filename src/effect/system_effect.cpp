#include "system_effect.h"


namespace game
{

	void EffectSystem::update(float delta)
	{
		auto views = _context.registry().view<CompEffect>();
		for (auto& ent : views)
		{
			auto& compEffect = views.get<CompEffect>(ent);
			float dtTicks = _context.frameTicker().deltaTicks();
			compEffect.tween.step(dtTicks);
			compEffect.tween.step(delta);
		}
	}

}