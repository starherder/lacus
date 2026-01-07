#pragma once

#include "game/scene/game_context.h"

#include "utility/dynamic_struct.h"
#include "utility/i_singleton.h"

namespace game 
{

	class ObjectFactory  : public utility::ISingleton<ObjectFactory>
	{
		using ConfigJsonMap = std::map<std::string, std::shared_ptr<nlohmann::json>>;

	public:
		ObjectFactory() = default;
		~ObjectFactory() = default;

		ObjectFactory(ObjectFactory&&) = delete;
		ObjectFactory(const ObjectFactory&) = delete;

		bool loadObjects(GameContext& context, const fs::path& rolescfg);

		bool loadSkills(GameContext& context, const fs::path& skillscfg);

		entt::entity createObject(const std::string& cfgid);

		entt::entity createRole(const std::string& cfgid);
	
		entt::entity createSkill(entt::entity owner, const std::string& cfgid);

		entt::entity createProjectile(const Vec2& source, const Vec2& target, float speed, 
					const std::string& tween_type, const std::string& particle);

		bool createParticleOnObject(entt::entity owner, const std::string& particle);

		void destroyObject(entt::entity entityid);

		const auto& getAllObjectCfgIds() { return _objectCfgIds; }
		const auto& getAllSkillCfgIds() { return _skillCfgIds; }

	private:
		bool loadRoleCfg(const std::string& id, const fs::path& rolescfg);

		bool loadSkillCfg(const std::string& id, const fs::path& skillcfg);

		std::optional<utility::Var> jsonToVar(const nlohmann::json& value);

	private:
		GameContext* _context = nullptr;

		std::vector<std::string> _objectCfgIds;
		std::vector<std::string> _skillCfgIds;

		ConfigJsonMap _jsonObjectCfgs;
		ConfigJsonMap _jsonSkillCfgs;
	};


}