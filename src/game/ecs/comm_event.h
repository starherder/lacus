#pragma once

#include "game/scene/game_context.h"



namespace game {

	struct MoveToGrid {
		entt::entity actor;
		Vec2i dest;
		bool findPath = false;
	};

	struct MotionStop {
		entt::entity actor;
	};


}