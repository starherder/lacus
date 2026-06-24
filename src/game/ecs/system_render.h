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
    
    void drawSceneQuadTree();
    
    void drawMarker();

    void drawObjects();

    void drawSkillEffect();

    void drawParticles();

    void drawSkyEffect();

    void drawFightText();

    void drawBubble();

    Color getBorderColor(CampSide side);
    Color getGroundColor(CampSide side);
    Color getForeColor(CampSide side);
    Color getFontColor(CampSide side);
};


}

