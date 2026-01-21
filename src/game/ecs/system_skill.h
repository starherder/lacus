#pragma once


#include "comm_system.h"
#include "comm_comp.h"
#include "comm_event.h"
#include "game/scene/game_camera.h"
#include "game/scene/game_scene.h"

#include "tweeny/tweeny.h"

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
        
        void startTraps(entt::entity srcid, entt::entity tarid, entt::entity skill);
        
        void startSprint(entt::entity srcid, entt::entity tarid, entt::entity skill);

        tweeny::tween<float, float> makeSkillSpellTween(const EvtCastSkillToObject& e);

        void onCastSkillToObject(const EvtCastSkillToObject& e);

        void onRoleUnderAttackEffect(const EvtRoleOnAttack& e);

        void onProjectileHitPos(const EvtProjectileHitPos& e);

        void onSkillEvent(const EvtExecSkillEvent& e);
    };




}