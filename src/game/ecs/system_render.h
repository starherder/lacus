#pragma once

#include "game/ecs/system_utils.h"


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
    
    void drawMarker();

    void drawObjects();

    void drawParticles();

    void drawSkyEffect();

    void drawFightText();
};


}

