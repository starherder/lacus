#pragma once

#include "game/ecs/system_utils.h"

namespace game
{
    class TriggerSystem : public EcsSystem
    {
    public:
        TriggerSystem(GameContext& context);
        ~TriggerSystem();

        void update(float delta) override;

    private:
        void onRoleEnterTriggerArea(const EvtRoleEnterTriggerArea& e);
    };
}
