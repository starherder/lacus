#pragma once


#include "ecs_system.h"
#include "comp_common.h"
#include "game/scene/game_camera.h"
#include "game/scene/game_scene.h"


namespace game {

class RenderSystem : public EcsSystem
{
public:
    RenderSystem(GameContext& context) : EcsSystem(context) {}
    ~RenderSystem() {}

    void draw();

private:
	void drawMotionDebug();
};


}

