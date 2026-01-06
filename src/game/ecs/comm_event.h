#pragma once

#include "game/scene/game_context.h"
#include "game/ecs/comp_fight.h"


namespace game {

	struct MoveToGrid {
		entt::entity actor;
		Vec2i dest;
		bool findPath = false;
	};

	struct MotionStop {
		entt::entity actor;
	};

	struct RoleCrossGrid {
		entt::entity actor;
		Vec2i lst_grid;
		Vec2i cur_grid;
	};

	struct RolePickItem {
		entt::entity actor;
		entt::entity item;
	};

	struct ObjectSelection {
		entt::entity object;
	};

	struct RoleExecSkillToPos {
		entt::entity source;
		Vec2 dest;
		entt::entity skill;
	};

	struct RoleExecSkillToObject {
		entt::entity source;
		entt::entity target;
		entt::entity skill;
	};

	struct ExecSkillEvent {
		entt::entity source;
		entt::entity skill;
		std::string event;
	};
}