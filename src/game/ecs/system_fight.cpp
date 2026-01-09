#include "system_fight.h"


namespace game
{

    FightSystem::FightSystem(GameContext& context) : EcsSystem(context)
    {
        _context.dispatcher().sink<RolePropAlter>().connect<&FightSystem::onRolePropAlter>(this);
        _context.dispatcher().sink<RoleOnAttack>().connect<&FightSystem::onRoleUnderAttack>(this);
        _context.dispatcher().sink<RoleLevelAlter>().connect<&FightSystem::onRoleLevelAlter>(this);
    }

    FightSystem::~FightSystem()
    {
    }

    void FightSystem::update(float delta)
    {

    }

    void FightSystem::onRoleUnderAttack(const RoleOnAttack& e)
    {
        // 计算伤害

        auto compName = _context.registry().get<CompNameId>(e.skill);
        auto compAffect = _context.registry().get<CompSkillAffect>(e.skill);
        
        auto funcs = parseFightFunc(compAffect.func);
        if (!funcs)
        {
            spdlog::error("skill: id({}) cfg({}), func config({}) error.", 
                (uint32_t)e.skill, compName.cfg_id, compAffect.func);
            return;
        }

        for (auto& fac : funcs.value())
        {
            applyFuncToTarget(fac, e.source, e.target);
        }
    }

    void FightSystem::onRolePropAlter(const RolePropAlter& e)
    {
        auto& base = _context.registry().get<CompBaseProp>(e.actor);
        auto& fight = _context.registry().get<CompFightProp>(e.actor);

        // TODO: 计算公式 ！！！！

        fight.hpm = base.cst * 10; // hp max
        fight.hpr = 0.1; // hp increase ratio
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

    void FightSystem::onRoleLevelAlter(const RoleLevelAlter& e)
    {
        int level = e.level;

        // TODO: 计算公式 ！！！！
        auto& base = _context.registry().get<CompBaseProp>(e.actor);
        base.cst += 10;
        base.str += 10;
        base.met += 10;
        base.met += 10;

        _context.dispatcher().trigger(RolePropAlter{ e.actor, true });
    }

    FightSystem::FuncFactorOpt FightSystem::parseFightFunc(const std::string& str)
    {
        // hp-,hp+20,hp-2*,hp+30%,buf+bleed,buf-poison

        std::vector<FuncFactor> result;

        auto& views = utility::StringUtil::split(str, ',');
        for (auto& svitem : views)
        {
            std::string sitem{ svitem.data(), svitem.size() };

            FuncFactor fac;
            size_t kpos = 0;
            size_t mpos = 0;
            size_t ppos = 0;

            for (size_t i = 0; i < sitem.size(); i++) {

                if (sitem[i] == '+' || sitem[i] == '-') {
                    kpos = i;
                    fac.operate = (sitem[i] == '+') ? FuncOperate::Plus : FuncOperate::Minus;
                }

                if (sitem[i] == '*') {
                    mpos = i;
                    fac.unit = FuncUnitType::Multi;
                }

                if (sitem[i] == '%') {
                    ppos = i;
                    fac.unit = FuncUnitType::Percent;
                }
            }

            if (kpos == 0) {
                return std::nullopt;
            }

            size_t vpos = 0;
            if (mpos != 0) {
                vpos = mpos;
            }
            else if (ppos != 0) {
                vpos = ppos;
            }
            else {
                vpos = sitem.size();
                fac.unit = FuncUnitType::Value;
            }

            fac.key = sitem.substr(0, kpos);

            std::string sval = sitem.substr(kpos + 1, vpos - kpos - 1);
            if (utility::StringUtil::is_number(sval)) {
                fac.fval = std::stof(sval);
            }
            else {
                fac.sval = sval;
            }

            result.push_back(fac);
        }

        return result;
    }

    void FightSystem::applyFuncToTarget(FuncFactor fac, entt::entity source, entt::entity target)
    {
        if (!_context.registry().valid(target)) {
            spdlog::error("applyFuncToTarget: target ({}) NOT valid", (uint32_t)target);
            return;
        }

        //auto& compBase = _context.registry().get<CompBaseProp>(target);
        auto& sourceFight = _context.registry().get<CompFightProp>(source);
        auto& targetFight = _context.registry().get<CompFightProp>(target);

        if (fac.key == "buf")
        {
            if (fac.operate == FuncOperate::Plus)
            {
                addBuf(target, fac.sval);
            }
            else {
                removeBuf(target, fac.sval);
            }
            return;
        }

        if (fac.key == "hp")
        {
            if (fac.fval == 0.0f) {
                fac.fval = sourceFight.atk;
            }
            else
            {
                if (fac.unit == FuncUnitType::Multi) {
                    fac.fval *= sourceFight.atk;
                }
                else if (fac.unit == FuncUnitType::Percent) {
                    fac.fval /= 100.0f;
                    fac.fval *= targetFight.hpm;
                }
                else {
                    fac.fval = fac.fval;
                }
            }
            
            if (fac.operate == FuncOperate::Minus) {
                fac.fval *= -1;
            }

            addHpToTarget(target, fac.fval);
        }
    }

    void FightSystem::addHpToTarget(entt::entity target, float hp)
    {
        auto& targetFight = _context.registry().get<CompFightProp>(target);
        targetFight.hp += hp;

        spdlog::info("target({}) hp {}{}", (uint32_t)target, hp>0?"+":"", hp);

        _context.dispatcher().trigger(RolHpAlter{ target, hp });
    }

    void FightSystem::addBuf(entt::entity target, const std::string& buf)
    {
        spdlog::info("target({}) add buff {}", (uint32_t)target, buf);
    }

    void FightSystem::removeBuf(entt::entity target, const std::string& buf)
    {
        spdlog::info("target({}) remove buff {}", (uint32_t)target, buf);
    }
}