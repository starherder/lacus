#include "system_fight.h"


namespace game
{

    DeclareEcsSystem(FightSystem, EcsPriority::Middle);

    FightSystem::FightSystem(GameContext& context) : EcsSystem(context)
    {
        _context.dispatcher().sink<EvtRoleOnAttack>().connect<&FightSystem::onRoleUnderAttack>(this);
        _context.dispatcher().sink<EvtExecPropFuncs>().connect<&FightSystem::applyAllFuncs>(this);
    }

    FightSystem::~FightSystem()
    {
    }

    void FightSystem::update(float delta)
    {
    }

    void FightSystem::onRoleUnderAttack(const EvtRoleOnAttack& e)
    {
        auto compName = _context.registry().get<CompNameId>(e.skill);
        auto compAffect = _context.registry().get<CompSkillAffect>(e.skill);
        
        EvtExecPropFuncs func;
        func.source = e.source;
        func.target = e.target;
        func.funcs = compAffect.func;
        _context.dispatcher().trigger(func);
    }

    void FightSystem::applyAllFuncs(const EvtExecPropFuncs& e)
    {
        auto funcs = SystemUtils::parseFightFunc(e.funcs);
        if (!funcs)
        {
            LogError("parse func config({}) error.", e.funcs);
            return;
        }

        for (auto& fac : funcs.value())
        {
            applyFuncToTarget(fac, e.source, e.target);
        }
    }

    void FightSystem::applyFuncToTarget(SystemUtils::FuncFactor fac, entt::entity source, entt::entity target)
    {
        if (!_context.registry().valid(target)) 
        {
            LogError("applyFuncToTarget: target ({}) NOT valid", (uint32_t)target);
            return;
        }

        //auto& compBase = _context.registry().get<CompBaseProp>(target);
        auto& targetFight = _context.registry().get<CompFightProp>(target);

        if (fac.key == "buf")
        {
            if (fac.operate == SystemUtils::FuncOperate::Plus)
            {
                addBuf(source, target, fac.sval);
            }
            else {
                removeBuf(target, fac.sval);
            }
            return;
        }

        if (fac.key == "hp")
        {
            if (fac.fval == 0.0f) 
            {
                auto& sourceFight = _context.registry().get<CompFightProp>(source);
                fac.fval = sourceFight.atk;
            }
            else
            {
                if (fac.unit == SystemUtils::FuncUnitType::Multi) 
                {
                    auto& sourceFight = _context.registry().get<CompFightProp>(source);
                    fac.fval *= sourceFight.atk;
                }
                else if (fac.unit == SystemUtils::FuncUnitType::Percent) 
                {
                    fac.fval /= 100.0f;
                    fac.fval *= targetFight.hpm;
                }
                else 
                {
                    fac.fval = fac.fval;
                }
            }
            
            if (fac.operate == SystemUtils::FuncOperate::Minus) 
            {
                fac.fval *= -1;
            }

            fac.fval = calcHpDamage(targetFight, fac.fval);

            addHpToTarget(target, fac.fval, source);
        }
    }

    float FightSystem::calcHpDamage(CompFightProp props, float hp)
    {
        if(hp < 0.0f)
        {
            // TODO: ÉËº¦¹«Ê½
            return  hp;
        } 

        return hp;
    }

    void FightSystem::addHpToTarget(entt::entity target, float hp, entt::entity source)
    {
        auto& targetTrans = _context.registry().get<CompTransform>(target);
        auto& targetFight = _context.registry().get<CompFightProp>(target);

        float hp_old = targetFight.hp;

        targetFight.hp += hp;
        targetFight.hp = std::clamp(targetFight.hp, 0.0f, targetFight.hpm);

        float dif_hp = targetFight.hp - hp_old;
        if(dif_hp !=0 )
        {
            showHpFloatingTip(target, dif_hp);
        }

        auto pdead = _context.registry().try_get<CompDead>(target);
        if(targetFight.hp <= 0.0f && pdead == nullptr) 
        {
            _context.dispatcher().trigger(EvtEnemyKilled{ source, target });

            _context.registry().emplace_or_replace<CompDead>(target, CompDead{source, 0});
        }

        _context.dispatcher().trigger(EvtRolHpAlter{ target, hp });
    }

    void FightSystem::addBuf(entt::entity source, entt::entity target, const std::string& buf)
    {
        //LogInfo("target({}) add buff {}", (uint32_t)target, buf);

        EvtAddBuff buff;
        buff.source = source;
        buff.target = target;
        buff.cfgid = buf;
        _context.dispatcher().trigger(buff);
    }

    void FightSystem::removeBuf(entt::entity target, const std::string& buf)
    {
        //LogInfo("target({}) remove buff {}", (uint32_t)target, buf);

        EvtRemoveBuff buff;
        buff.target = target;
        buff.cfgid = buf;
        _context.dispatcher().trigger(buff);
    }

    void FightSystem::showHpFloatingTip(entt::entity target, float hp)
    {
        EvtShowFloatText ft;
        ft.actor = target;
        ft.type = FloatTextType::HP;
        ft.val = (int)hp;
        _context.dispatcher().trigger(ft);
    }
}