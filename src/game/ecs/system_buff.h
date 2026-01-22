#pragma once


#include "system_utils.h"

namespace game
{

    class BuffSystem : public EcsSystem
    {
    public:
        BuffSystem(GameContext& context);
        ~BuffSystem();

        void update(float delta) override;

    private:
        void onAddBuffToObject(const EvtAddBuff& e);

        void onRemoveBuffFromObject(const EvtRemoveBuff& e);

        void onPeriodExec(entt::entity target, entt::entity buff, entt::entity source);
    };

}