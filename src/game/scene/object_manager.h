#pragma once

#include "game/scene/game_context.h"
#include "game/ecs/comm_comp.h"

#include "utility/dynamic_struct.h"
#include "utility/i_singleton.h"

namespace game 
{

	class ObjectManager final : public utility::ISingleton<ObjectManager>
	{
		using ConfigJsonMap = std::map<std::string, JsonPtr>;

	public:
		ObjectManager() = default;
		~ObjectManager() = default;

		ObjectManager(ObjectManager&&) = delete;
		ObjectManager(const ObjectManager&) = delete;

		void init(GameContext* context);
		void reloadAll();

		bool loadItems(const fs::path& itemdir);
		bool loadRoles(const fs::path& roledir);
		bool loadEnemies(const fs::path& roledir);
		bool loadOther(const fs::path& roledir);
		bool loadSkills(const fs::path& skilldif);
		bool loadBuffs(const fs::path& buffdir);

		entt::entity createObject(const std::string& cfgid);

		entt::entity createSkill(entt::entity owner, const std::string& cfgid);

		entt::entity createBuff(entt::entity owner, const std::string& cfgid);

		entt::entity createProjectile(const Vec2& source, const Vec2& target, const struct CompProjectileCfg& cfg);

		entt::entity createTrap(const Vec2& target, float range, const Color& color, 
			const std::string& texture, const std::string& particle, ShapeType shape_type);

		particle::ParticlePtr createParticleOnObject(entt::entity owner, const std::string& particle);

		void createSkyEffect(SkyEffect effect,  Color color, int last,int fadein = 500, int fadeout = 500);

		void destroyObject(entt::entity entityid);

		const auto& getAllObjectCfgIds() { return _objectCfgIds; }
		const auto& getAllRoleCfgIds() { return _roleCfgIds; }
		const auto& getAllEnemyCfgIds() { return _enemyCfgIds; }

		const auto& getAllSkillCfgIds() { return _skillCfgIds; }
		const auto& getAllBuffCfgIds() { return _buffCfgIds; }

		bool findObjectCfg(const std::string& cfgid);

		const Properties& getObjectCfgProperties(const std::string& cfgid);

	private:
		std::string loadObjectCfg(const fs::path& rolescfg);
		std::string loadSkillCfg(const fs::path& skillcfg);
		std::string loadBuffCfg(const fs::path& skillcfg);

		entt::entity createObject(const nJson& value);
		entt::entity createRole(const nJson& json);
		entt::entity createSpawner(const nJson& json);

		std::optional<utility::Var> jsonToVar(const nlohmann::json& value);

	private:
		GameContext* _context = nullptr;

		std::vector<std::string> _roleCfgIds;
		std::vector<std::string> _enemyCfgIds;
		std::vector<std::string> _otherCfgIds;
		std::vector<std::string> _skillCfgIds;
		std::vector<std::string> _buffCfgIds;
		std::vector<std::string> _objectCfgIds;

		ConfigJsonMap _jsonObjectCfgs;
		ConfigJsonMap _jsonSkillCfgs;
		ConfigJsonMap _jsonBuffCfgs;

		fs::path _skillPath;
		fs::path _buffPath;
		fs::path _itemPath;
		fs::path _rolePath;
		fs::path _enemyPath;
		fs::path _otherPath;
	};


}