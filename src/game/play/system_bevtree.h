#pragma once


#include "ecs_system.h"
#include "comp_common.h"
#include "game/scene/game_camera.h"
#include "game/scene/game_scene.h"


namespace game {

class BevTreeSystem : public EcsSystem
{
public:
    BevTreeSystem(GameContext& context) : EcsSystem(context) {}
    ~BevTreeSystem() {}

    void update(float delta) override;
};


}

