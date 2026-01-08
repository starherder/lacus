#include "effect_manager.h"


namespace game
{

	bool SubEffectGroup::create(const nlohmann::json& js)
	{
		return true;
	}

	void SubEffectGroup::execute()
	{

	}


	EffectManager::EffectManager(GameContext& context) : _context(context)
	{
	}

	EffectManager::~EffectManager()
	{
	}

	bool EffectManager::load(const fs::path& cfgfile)
	{
		return false;
	}
	
	void EffectManager::reload()
	{
	}


	bool EffectManager::createEffectOnEntity(entt::entity ent, const std::string& effect_name)
	{
		_context.registry().emplace<CompEffect>(ent, CompEffect{});
		auto& compEffect = _context.registry().get<CompEffect>(ent);

		//compEffect.tween.

		// effect的配置，一个个放到tween里，用于定时顺序执行

		return true;
	}

}