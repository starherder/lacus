#pragma once


#include "comm_system.h"
#include "comm_comp.h"
#include "comm_event.h"
#include "game/scene/game_camera.h"
#include "game/scene/game_scene.h"


namespace game {

    class PickupSystem : public EcsSystem
    {
    public:
        PickupSystem(GameContext& context);
        ~PickupSystem();

        void update(float deltaTime);

    private:
        void onEventMoveToGrid(const RoleCrossGrid& e);
        void pickUp(entt::entity role, entt::entity obj, const Vec2i& grid);
    };
}

