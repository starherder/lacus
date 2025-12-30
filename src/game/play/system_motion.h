#pragma once


#include "ecs_system.h"
#include "comp_common.h"
#include "game/scene/game_camera.h"
#include "game/scene/game_scene.h"


namespace game {

	class MotionSystem : public EcsSystem
	{
	public:
        MotionSystem(GameContext& context) : EcsSystem(context) {};
        ~MotionSystem() {}

        void update(float deltaTime);

        void setActorPos(entt::entity id, const Vec2& pos);

        Vec2 getActorPos(entt::entity id);

        void setMotionSpeed(entt::entity id, float speed);

        float getMotionSpeed(entt::entity id);

        bool reachGridCenter(const Vec2& pos, const Vec2i& grid, float epsilon);

        bool motionStart(entt::entity id, const Vec2& dst);

        bool motionStop(entt::entity id);

        bool motionPause(entt::entity id, bool pause);

	};


}

