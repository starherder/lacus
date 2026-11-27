#pragma once


#include "game/ecs/system_utils.h"

#include "tweeny/tweeny.h"

#include <memory>
#include <set>
#include <vector>

namespace game 
{

    class SkillSystem : public EcsSystem
    {
    public:
        SkillSystem(GameContext& context);
        ~SkillSystem();

        void update(float delta) override;

    private:
        void skillApplyToTarget(const EvtCastSkillToObject& e);

        void startProjectile(entt::entity source, entt::entity target, entt::entity skill);

        void fireProjectileToTarget(entt::entity source, entt::entity target, entt::entity skill,
            std::shared_ptr<std::set<entt::entity>> hitTargets = nullptr);

        void updateProjectileBursts(int deltaTicks);

        std::vector<entt::entity> findProjectileTargets(entt::entity source, entt::entity firstTarget, entt::entity skill, int count);

        bool canProjectileAttackTarget(entt::entity source, entt::entity target, entt::entity skill);
        
        void startTraps(entt::entity srcid, entt::entity tarid, entt::entity skill);

        void startWave(entt::entity srcid, entt::entity tarid, entt::entity skill);

        void startSprint(entt::entity srcid, entt::entity tarid, entt::entity skill);

        void startLightning(entt::entity srcid, entt::entity tarid, entt::entity skill);

        void onCastSkillToObject(const EvtCastSkillToObject& e);

        void onCastSkillToPos(const EvtCastSkillToPos& e);

        void onRoleUnderAttackEffect(const EvtRoleOnAttack& e);

        void onSkillHitPos(const EvtSkillHitPos& e);

        void onSkillHitTarget(const EvtSkillHitTarget& e);

        void onTrapPeriodExec(entt::entity srcid, entt::entity skill, entt::entity trap);

        void onSkillEvent(const EvtExecSkillEvent& e);

        void onSkillFinish(entt::entity srcid, entt::entity skill, bool success);

        tweeny::tween<float, float> makeSkillSpellTween(const EvtCastSkillToObject& e);

        entt::entity createTrapAtPos(entt::entity srcid, const Vec2& target, entt::entity skill, ShapeType shape_type);

        void createWaveRange(int r, entt::entity srcid, const Vec2& targetPos, entt::entity skill);

        void createLightningToTarget(entt::entity srcid, entt::entity target, entt::entity skill);
    };


}
