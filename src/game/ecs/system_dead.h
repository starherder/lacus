#pragma once


#include "comm_system.h"
#include "comm_comp.h"
#include "comm_event.h"
#include "game/scene/game_camera.h"
#include "game/scene/game_scene.h"


namespace game 
{

    class DeadSystem : public EcsSystem
    {
    public:
        DeadSystem(GameContext& context) : EcsSystem(context) {}
        ~DeadSystem() {}

        void update(float delta) override;

    };




}