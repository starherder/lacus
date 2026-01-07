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

	struct RolePickItemStart {
		entt::entity actor;
		entt::entity item;
	};

	struct RolePickItemFinish {
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

	struct RoleOnAttack {
		entt::entity source;
		entt::entity target;
		entt::entity skill;
	};

	struct ProjectileHitPos {
		entt::entity source;
		entt::entity skill;
		//entt::entity projectile;
		Vec2 pos;
	};

	struct ExecSkillEvent {
		entt::entity source;
		entt::entity skill;
		std::string event;
	};
}