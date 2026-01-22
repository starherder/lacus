#pragma once

#include "game/scene/game_context.h"
#include "game/ecs/comm_comp.h"

#include "utility/dynamic_struct.h"
#include "utility/i_singleton.h"

namespace game 
{

	class ObjectFactory  : public utility::ISingleton<ObjectFactory>
	{
		using ConfigJsonMap = std::map<std::string, JsonPtr>;

	public:
		ObjectFactory() = default;
		~ObjectFactory() = default;

		ObjectFactory(ObjectFactory&&) = delete;
		ObjectFactory(const ObjectFactory&) = delete;

		bool loadObjects(GameContext& context, const fs::path& roledir);
		bool loadSkills(GameContext& context, const fs::path& skilldif);
		bool loadBuffs(GameContext& context, const fs::path& buffdir);

		entt::entity createActor(const std::string& cfgid);

		entt::entity createSkill(entt::entity owner, const std::string& cfgid);
		entt::entity createBuff(entt::entity owner, const std::string& cfgid);

		entt::entity createProjectile(const Vec2& source, const Vec2& target, const std::string& particle);

		entt::entity createTrap(const Vec2& target, float range, const Color& color, 
			const std::string& texture, const std::string& particle, ShapeType shape_type);

		//entt::entity createWave()

		particle::ParticlePtr createParticleOnObject(entt::entity owner, const std::string& particle);

		void createSkyEffect(SkyEffect effect,  Color color, int last,int fadein = 500, int fadeout = 500);

		void destroyObject(entt::entity entityid);

		const auto& getAllObjectCfgIds() { return _objectCfgIds; }
		const auto& getAllSkillCfgIds() { return _skillCfgIds; }

		const Properties& getObjectCfgProperties(const std::string& cfgid);

	private:
		bool loadRoleCfg(const fs::path& rolescfg);
		bool loadSkillCfg(const fs::path& skillcfg);
		bool loadBuffCfg(const fs::path& skillcfg);

		entt::entity createObject(const nJson& value);
		entt::entity createRole(const nJson& json);
		entt::entity createSpawner(const nJson& json);

		std::optional<utility::Var> jsonToVar(const nlohmann::json& value);

	private:
		GameContext* _context = nullptr;

		std::vector<std::string> _objectCfgIds;
		std::vector<std::string> _skillCfgIds;
		std::vector<std::string> _buffCfgIds;

		ConfigJsonMap _jsonObjectCfgs;
		ConfigJsonMap _jsonSkillCfgs;
		ConfigJsonMap _jsonBuffCfgs;
	};


}