#pragma once

#include "game/scene/game_context.h"
#include "game/ecs/comp_fight.h"


namespace game 
{
	struct EvtRoleDead {
		entt::entity actor;
	};

	struct EvtRoleDestroyed {
		entt::entity actor;
	};

	struct EvtRolePropAlter {
		entt::entity actor;
		bool reset_hp = false;
	};

	struct EvtRoleLevelAlter {
		entt::entity actor;
		int level = 0;
	};

	struct EvtEnemyKilled {
		entt::entity actor;
		entt::entity enemy;
	};

	struct EvtRolHpAlter {
		entt::entity actor;
		float diff = 0;
	};

	struct EvtMoveToGrid {
		entt::entity actor;
		Vec2i dest;
		bool findPath = false;
	};

	struct EvtMotionStop {
		entt::entity actor;
	};

	struct EvtRoleCrossGrid {
		entt::entity actor;
		Vec2i lst_grid;
		Vec2i cur_grid;
	};

	struct EvtRolePickItemStart {
		entt::entity actor;
		entt::entity item;
	};

	struct EvtRolePickItemFinish {
		entt::entity actor;
		entt::entity item;
	};

	struct EvtObjectSelection {
		entt::entity object;
	};

	struct EvtCastSkillToObject {
		entt::entity source;
		entt::entity target;
		entt::entity skill;
	};

	struct EvtCastSkillToPos {
		entt::entity source;
		Vec2 targetPos;
		entt::entity skill;
	};

	struct EvtAddBuff {
		entt::entity source;
		entt::entity target;
		std::string cfgid;
	};

	struct EvtRemoveBuff {
		entt::entity target;
		std::string cfgid;
	};

	struct EvtRoleOnAttack {
		entt::entity source;
		entt::entity target;
		entt::entity skill;
	};

	struct EvtRoleOnDrop {
		entt::entity actor;
		Vec2 pos;
	};

	struct EvtProjectileHitPos {
		entt::entity source;
		entt::entity skill;
		//entt::entity projectile;
		Vec2 pos;
	};

	struct EvtExecSkillEvent {
		entt::entity source;
		entt::entity skill;
		std::string event;
	};

	struct EvtExecPropFuncs {
		entt::entity source;
		entt::entity target;
		std::string funcs;
	};
}