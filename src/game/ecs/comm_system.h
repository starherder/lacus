#pragma once

#include "game/scene/game_context.h"

namespace game {

	enum class EcsPriority 
	{	
		MotionSystem,
		RenderSystem,
		BevTreeSystem,

		MaxSystem,
	};


	class EcsSystem 
	{
	public:
		EcsSystem() = delete;
		EcsSystem(EcsSystem&&) = delete;
		EcsSystem(const EcsSystem&) = delete;

		EcsSystem(GameContext& context) : _context(context) {}
		virtual ~EcsSystem() = default;

		virtual void update(float delta) {};

		virtual void draw() {};

	protected:
		GameContext& _context;
	};

}