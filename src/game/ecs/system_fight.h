#pragma once


#include "comm_system.h"
#include "comm_comp.h"
#include "comm_event.h"
#include "game/scene/game_camera.h"
#include "game/scene/game_scene.h"


namespace game 
{

    class FightSystem : public EcsSystem
    {
    public:
        FightSystem(GameContext& context) : EcsSystem(context) {}
        ~FightSystem() {}

        void update(float delta) override;

    };




}