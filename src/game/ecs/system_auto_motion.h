#pragma once


#include "system_utils.h"


namespace game {

    class AutoMotionSystem : public EcsSystem
    {
    public:
        AutoMotionSystem(GameContext& context);
        ~AutoMotionSystem();

        void update(float deltaTime);

        bool motionStart(entt::entity id, const Vec2i& grid, bool findPath = true);

        bool motionStop(entt::entity id);

        bool motionPause(entt::entity id, bool pause);

    private:
        void onEventMoveToGrid(const EvtMoveToGrid& e);
        void onEventStopMotion(const EvtRoleStopMotion& e);
        void onEventMotionStateSwtich(const EvtMotionSwitchState& e);

        bool tweenNextGrid(entt::entity id);

        void checkEntityGrid(entt::entity ent, const Vec2& curpos);
    };

}

