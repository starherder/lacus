#pragma once


#include "game/ecs/system_utils.h"


namespace game {

    class PickupSystem : public EcsSystem
    {
    public:
        PickupSystem(GameContext& context);
        ~PickupSystem();

        void update(float deltaTime);

    private:
        void onEventMoveToGrid(const EvtRoleCrossGrid& e);

        void pickRange(entt::entity role);

        void pickUp(entt::entity role, entt::entity obj);
        void useItem(entt::entity actor, entt::entity obj);
    };

}

