#pragma once

#include "system_utils.h"


namespace game {

class BubbleSystem : public EcsSystem
{
public:
    BubbleSystem(GameContext& context);
    ~BubbleSystem();

    void update(float delta) override;
    bool updateWhenGamePaused() const override { return true; }

private:
    void onShowBubble(const EvtShowBubble& e);
};

}

