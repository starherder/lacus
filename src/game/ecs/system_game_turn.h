#pragma once

#include "game/ecs/system_utils.h"

namespace game {

    class GameTurnSystem : public EcsSystem
    {
    public:
        GameTurnSystem(GameContext& context);
        ~GameTurnSystem();

        void update(float deltaTime);

    };

}

