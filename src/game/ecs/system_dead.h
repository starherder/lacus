#pragma once


#include "system_utils.h"


namespace game 
{

    class DeadSystem : public EcsSystem
    {
    public:
        DeadSystem(GameContext& context) : EcsSystem(context) {}
        ~DeadSystem() {}

        void update(float delta) override;

    };
}