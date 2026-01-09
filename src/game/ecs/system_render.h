#pragma once


#include "comm_system.h"
#include "comm_comp.h"
#include "game/scene/game_camera.h"
#include "game/scene/game_scene.h"


namespace game {

class RenderSystem : public EcsSystem
{
public:
    RenderSystem(GameContext& context) : EcsSystem(context) {}
    ~RenderSystem() {}

    void update(float delta);

    void draw();

private:
	void drawMotionDebug();

    void drawSceneDebug();

    void drawObjects();

    void drawParticles();

    void drawSkyEffect();

    void drawFightText();
};


}

