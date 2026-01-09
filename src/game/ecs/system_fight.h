#pragma once


#include "comm_system.h"
#include "comm_comp.h"
#include "comm_event.h"
#include "game/scene/game_camera.h"
#include "game/scene/game_scene.h"

#include "tweeny/tweeny.h"

namespace game 
{

    class FightSystem : public EcsSystem
    {
    public:
        FightSystem(GameContext& context);
        ~FightSystem();

        void update(float delta) override;

    private:
        void onCastSkillToObject(const CastSkillToObject& e);

        void skillAffectApplyToObject(const CastSkillToObject& e);

        void onRoleUnderAttack(const RoleOnAttack& e);

        void startProjectileObject(entt::entity source, entt::entity target, entt::entity skill);
        
        void onProjectileHitPos(const ProjectileHitPos& e);

        void onSkillEvent(const ExecSkillEvent& e);

        tweeny::tween<float, float> makeSkillTween(const CastSkillToObject& e);
    };




}