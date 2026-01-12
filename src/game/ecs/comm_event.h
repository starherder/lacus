#pragma once

#include "game/scene/game_context.h"
#include "game/ecs/comp_fight.h"


namespace game {
	struct RolePropAlter {
		entt::entity actor;
		bool reset_hp = false;
	};

	struct RoleLevelAlter {
		entt::entity actor;
		int level = 0;
	};

	struct RolHpAlter {
		entt::entity actor;
		float diff = 0;
	};

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

	struct CastSkillToObject {
		entt::entity source;
		entt::entity target;
		entt::entity skill;
	};

	struct AddBuffToObject {
		entt::entity source;
		entt::entity target;
		std::string cfgid;
	};

	struct RemoveBuffFromObject {
		entt::entity target;
		std::string cfgid;
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

	struct AddFuncsToTarget {
		entt::entity source;
		entt::entity target;
		std::string funcs;
	};
}