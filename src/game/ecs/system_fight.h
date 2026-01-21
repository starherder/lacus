#pragma once


#include "system_utils.h"

namespace game
{

    class FightSystem : public EcsSystem
    {
    public:
        FightSystem(GameContext& context);
        ~FightSystem();

        void update(float delta) override;

    private:
        void onRoleUnderAttack(const EvtRoleOnAttack& e);

        void applyFuncToTarget(SystemUtils::FuncFactor fac, entt::entity source, entt::entity target);

        void addBuf(entt::entity source, entt::entity target, const std::string& buf);

        void removeBuf(entt::entity target, const std::string& buf);

        void addHpToTarget(entt::entity target, float hp, entt::entity source);

        void showHpFloatingTip(entt::entity target, float hp);

        float calcHpDamage(CompFightProp props, float hp);

        void applyAllFuncs(const EvtAddPropFuncs& e);
    };



}