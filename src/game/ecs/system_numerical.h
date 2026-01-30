#pragma once


#include "system_utils.h"


namespace game 
{

    class NumericalSystem : public EcsSystem
    {
    public:
        NumericalSystem(GameContext& context);
        ~NumericalSystem();

        void onRolePropAlter(const EvtRolePropAlter& e);

        void onRoleLevelAlter(const EvtRoleLevelAlter& e);

        void onRoleKillEnemy(const EvtEnemyKilled& e);

        void update(float delta) override;

    private:
        int getLevelupExp(int level);

        int getKillAwardExp(int enemy_level);
    };


}