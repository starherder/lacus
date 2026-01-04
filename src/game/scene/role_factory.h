#pragma once

#include "game/scene/game_context.h"

#include "utility/dynamic_struct.h"
#include "utility/i_singleton.h"

namespace game 
{

	class RoleFactory  : public utility::ISingleton<RoleFactory>
	{
	public:
		RoleFactory() = default;
		~RoleFactory() = default;

		RoleFactory(RoleFactory&&) = delete;
		RoleFactory(const RoleFactory&) = delete;

		bool load(GameContext& context, const fs::path& rolescfg);
	
		entt::entity createRole(const std::string& cfgid);
	
		void destroyRole(entt::entity entityid);

	private:
		bool loadRoleCfg(const std::string& id, const fs::path& cfgfile);

		std::optional<utility::Var> jsonToVar(const nlohmann::json& value);

	private:
		GameContext* _context = nullptr;

		std::map<std::string, std::shared_ptr<nlohmann::json>> _jsonCfgs;
	};


}