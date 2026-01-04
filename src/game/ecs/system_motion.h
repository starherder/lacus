#pragma once


#include "comm_system.h"
#include "comm_comp.h"
#include "comm_event.h"
#include "game/scene/game_camera.h"
#include "game/scene/game_scene.h"


namespace game {

    class MotionSystem : public EcsSystem
    {
    public:
        MotionSystem(GameContext& context);
        ~MotionSystem();

        void update(float deltaTime);

        bool motionStart(entt::entity id, const Vec2i& grid, bool findPath = true);

        bool motionStop(entt::entity id);

        bool motionPause(entt::entity id, bool pause);

    private:
        void onEventMoveToGrid(const MoveToGrid& e);

        bool tweenNextGrid(entt::entity id);
    };
}

