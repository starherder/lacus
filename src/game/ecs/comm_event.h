#pragma once

#include "game/scene/game_context.h"



namespace game {

	struct MoveToPos {
		entt::entity actor;
		Vec2 dest;
	};

	struct MotionStop {
		entt::entity actor;
	};


}