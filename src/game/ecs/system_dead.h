#pragma once


#include "system_utils.h"


namespace game 
{

    class DeadSystem : public EcsSystem
    {
    public:
        DeadSystem(GameContext& context);
        ~DeadSystem();

        void update(float delta) override;

    private:
        void onRoleDead(const EvtRoleDead& e);

        void dropAllItems(entt::entity actor);
    };
}