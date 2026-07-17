#pragma once

#include "system_utils.h"

namespace game
{
    class LevelSystem : public EcsSystem
    {
    public:
        LevelSystem(GameContext& context);
        ~LevelSystem();

    private:
        void onRoleDead(const EvtRoleDead& e);
    };
}
