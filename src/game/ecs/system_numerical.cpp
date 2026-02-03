#include "system_numerical.h"

#include "game/game_script.h"

namespace game 
{
    DeclareEcsSystem(NumericalSystem, EcsPriority::Middle);


    NumericalSystem::NumericalSystem(GameContext& context) : EcsSystem(context)
	{
		_context.dispatcher().sink<EvtRolePropAlter>().connect<&NumericalSystem::onRolePropAlter>(this);
        _context.dispatcher().sink<EvtRoleLevelAlter>().connect<&NumericalSystem::onRoleLevelAlter>(this);
        _context.dispatcher().sink<EvtEnemyKilled>().connect<&NumericalSystem::onRoleKillEnemy>(this);
    }

    NumericalSystem::~NumericalSystem()
	{
	}
	
	void NumericalSystem::update(float delta)
	{
	}

    void NumericalSystem::onRolePropAlter(const EvtRolePropAlter& e)
    {
        auto& base = _context.registry().get<CompBaseProp>(e.actor);
        auto& fight = _context.registry().get<CompFightProp>(e.actor);

        fight = _context.gameScript().call<CompFightProp>("get_fight_prop", base);

        if (e.reset_hp)
        {
            fight.hp = fight.hpm;
        }
    }

    void NumericalSystem::onRoleLevelAlter(const EvtRoleLevelAlter& e)
    {
        auto pBaseProp = _context.registry().try_get<CompBaseProp>(e.actor);
        if (!pBaseProp)
        {
            LogError("actor {} CompBaseProp NOT found.", (uint32_t)e.actor);
            return;
        }

        auto& base = _context.registry().get<CompBaseProp>(e.actor);
        base.lv += e.level;

        base = _context.gameScript().call<CompBaseProp>("get_level_baseprop", base);

        _context.dispatcher().trigger(EvtRolePropAlter{ e.actor, true });
    }

    void NumericalSystem::onRoleKillEnemy(const EvtEnemyKilled& e)
    {
        // 给e.actor加经验，必要时升级
        auto pEnemyProp = _context.registry().try_get<CompBaseProp>(e.enemy);
        auto pActorProp = _context.registry().try_get<CompBaseProp>(e.actor);
        if (!pEnemyProp || !pActorProp)
        {
            LogError("CompBaseProp NOT found.");
            return;
        }
        
        auto awardExp = getKillAwardExp(pEnemyProp->lv);
        awardExp += pActorProp->exp;

        int lvup = 0;
        while (true)
        {
            auto needExp = getLevelupExp(pActorProp->lv);
            if (awardExp > needExp)
            {
                lvup++;
                awardExp -= needExp;
            }
            else
            {
                break;
            }
        }

        pActorProp->exp = awardExp;

        if (lvup > 0)
        {
            _context.dispatcher().trigger(EvtRoleLevelAlter{ e.actor, lvup });
            _context.dispatcher().trigger(EvtShowFloatText{ e.actor, FloatTextType::LV, lvup });
        }
    }

    int NumericalSystem::getLevelupExp(int level)
    {
        return _context.gameScript().call<int>("get_levelup_exp", level);
    }

    int NumericalSystem::getKillAwardExp(int enemy_level)
    {
        return _context.gameScript().call<int>("get_kill_award_exp", enemy_level);
    }
}