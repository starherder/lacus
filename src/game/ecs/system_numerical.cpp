#include "system_numerical.h"



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

        // TODO: 计算公式 ！！！！

        fight.hpm = base.cst * 10; // hp max
        fight.hpr = 0.1f; // hp increase ratio
        if (e.reset_hp) fight.hp = fight.hpm;

        fight.atk = base.cst / 3 + base.str;
        fight.def = base.cst + base.str / 3;
        fight.mvs = base.dex;
        fight.ats = base.dex;
        fight.atd = base.dex + base.str;
        fight.crt = base.dex / 10000;
        fight.par = base.dex / 10000;

        // fight.xxx += buf.xxx // buf
        // fight.xxx += equip.xxx // 装备
        // fight.xxx + carrier.xxx // 载具
    }

    void NumericalSystem::onRoleLevelAlter(const EvtRoleLevelAlter& e)
    {
        auto pBaseProp = _context.registry().try_get<CompBaseProp>(e.actor);
        if (!pBaseProp)
        {
            SPDLOG_ERROR("actor {} CompBaseProp NOT found.", (uint32_t)e.actor);
            return;
        }

        // TODO: 计算公式 ！！！！
        auto& base = _context.registry().get<CompBaseProp>(e.actor);
        base.lv += e.level;
        base.cst += 10 * e.level;
        base.str += 10 * e.level;
        base.met += 10 * e.level;
        base.met += 10 * e.level;

        _context.dispatcher().trigger(EvtRolePropAlter{ e.actor, true });
    }

    void NumericalSystem::onRoleKillEnemy(const EvtEnemyKilled& e)
    {
        // 给e.actor加经验，必要时升级
        auto pEnemyProp = _context.registry().try_get<CompBaseProp>(e.enemy);
        auto pActorProp = _context.registry().try_get<CompBaseProp>(e.actor);
        if (!pEnemyProp || !pActorProp)
        {
            SPDLOG_ERROR("CompBaseProp NOT found.");
            return;
        }
        
        auto awardExp = enemyDeadDropExp(pEnemyProp->lv);
        awardExp += pActorProp->exp;

        int lvup = 0;
        while (true)
        {
            auto needExp = levelupExp(pActorProp->lv);
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

    int NumericalSystem::levelupExp(int level)
    {
        //TODO: 公式
        return 50 + level * 100;
    }

    int NumericalSystem::totalExp(int level)
    {
        int total = 0;
        for (int l = 0; l < level; l++)
        {
            total += levelupExp(l);
        }
        return total;
    }

    int NumericalSystem::enemyDeadDropExp(int enemy_level)
    {
        //TODO: 公式
        return (int)(totalExp(enemy_level) / 2.0f);
    }
}