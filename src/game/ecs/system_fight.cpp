#include "system_fight.h"


namespace game
{

    FightSystem::FightSystem(GameContext& context) : EcsSystem(context)
    {
        _context.dispatcher().sink<RolePropAlter>().connect<&FightSystem::onRolePropAlter>(this);
        _context.dispatcher().sink<RoleOnAttack>().connect<&FightSystem::onRoleUnderAttack>(this);
        _context.dispatcher().sink<RoleLevelAlter>().connect<&FightSystem::onRoleLevelAlter>(this);
        _context.dispatcher().sink<AddFuncsToTarget>().connect<&FightSystem::applyAllFuncs>(this);

    }

    FightSystem::~FightSystem()
    {
    }

    void FightSystem::update(float delta)
    {
        auto views = _context.registry().view<CompFightText>();
        for (auto& ent : views)
        {
            auto& ft = views.get<CompFightText>(ent);

            auto delta = _context.frameTicker().deltaTicks();
            ft.tween.step(delta);
        }
    }

    void FightSystem::onRoleUnderAttack(const RoleOnAttack& e)
    {
        // 计算伤害

        auto compName = _context.registry().get<CompNameId>(e.skill);
        auto compAffect = _context.registry().get<CompSkillAffect>(e.skill);
        
        AddFuncsToTarget func;
        func.source = e.source;
        func.target = e.target;
        func.funcs = compAffect.func;
        _context.dispatcher().trigger(func);
    }


    void FightSystem::applyAllFuncs(const AddFuncsToTarget& e)
    {
        auto funcs = SystemUtils::parseFightFunc(e.funcs);
        if (!funcs)
        {
            spdlog::error("parse func config({}) error.", e.funcs);
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

    void FightSystem::applyFuncToTarget(SystemUtils::FuncFactor fac, entt::entity source, entt::entity target)
    {
        if (!_context.registry().valid(target)) {
            spdlog::error("applyFuncToTarget: target ({}) NOT valid", (uint32_t)target);
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
            if (fac.fval == 0.0f) {
                auto& sourceFight = _context.registry().get<CompFightProp>(source);
                fac.fval = sourceFight.atk;
            }
            else
            {
                if (fac.unit == SystemUtils::FuncUnitType::Multi) {
                    auto& sourceFight = _context.registry().get<CompFightProp>(source);
                    fac.fval *= sourceFight.atk;
                }
                else if (fac.unit == SystemUtils::FuncUnitType::Percent) {
                    fac.fval /= 100.0f;
                    fac.fval *= targetFight.hpm;
                }
                else {
                    fac.fval = fac.fval;
                }
            }
            
            if (fac.operate == SystemUtils::FuncOperate::Minus) {
                fac.fval *= -1;
            }

            fac.fval = calcHpDamage(targetFight, fac.fval);

            addHpToTarget(target, fac.fval);
        }
    }

    float FightSystem::calcHpDamage(CompFightProp props, float hp)
    {
        if(hp < 0.0f)
        {
            // TODO: 伤害公式
            return  hp;
        } 

        return hp;
    }

    void FightSystem::addHpToTarget(entt::entity target, float hp)
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
            _context.registry().emplace<CompDead>(target);    
        }

        _context.dispatcher().trigger(RolHpAlter{ target, hp });
    }

    void FightSystem::addBuf(entt::entity source, entt::entity target, const std::string& buf)
    {
        spdlog::info("target({}) add buff {}", (uint32_t)target, buf);

        AddBuffToObject buff;
        buff.source = source;
        buff.target = target;
        buff.cfgid = buf;
        _context.dispatcher().trigger(buff);
    }

    void FightSystem::removeBuf(entt::entity target, const std::string& buf)
    {
        spdlog::info("target({}) remove buff {}", (uint32_t)target, buf);

        RemoveBuffFromObject buff;
        buff.target = target;
        buff.cfgid = buf;
        _context.dispatcher().trigger(buff);
    }

    void FightSystem::showHpFloatingTip(entt::entity target, float hp)
    {
        auto& targetTrans = _context.registry().get<CompTransform>(target);
        float curY = targetTrans.position.y;
        float dstY = targetTrans.position.y - 100;

        spdlog::info("target({}) hp {}{}", (uint32_t)target, hp > 0 ? "+" : "", hp);

        auto word = _context.registry().create();
        _context.registry().emplace<CompTransform>(word, targetTrans);
        _context.registry().emplace<CompFightText>(word, CompFightText{});

        auto& ft = _context.registry().get<CompFightText>(word);

        auto& fontname = _context.gameConfig().float_text.font_name;
        auto& fontsize = _context.gameConfig().float_text.font_size;
        auto& colorinc = _context.gameConfig().float_text.color_inc_hp;
        auto& colordec = _context.gameConfig().float_text.color_dec_hp;
        auto& tweenMode = _context.gameConfig().float_text.tween_mode;
        auto& floatticks = _context.gameConfig().float_text.float_ticks;

        ft.font = _context.fontMgr().get(HashString(fontname.c_str()), fontsize);
        ft.color = (hp > 0) ? colorinc : colordec;

        ft.text = fmt::format("HP{}{}", hp > 0 ? "+" : "", (int)hp);
        ft.tween = tweeny::from(curY, 255.0f)
            .to(dstY, 0.0f)
            .via(tweenMode)
            .during(floatticks)
            .onStep([this, word](auto& t, float y, float a) {
            if (t.isFinished()) {
                _context.registry().emplace<CompDestroy>(word);
                return true;
            }

            auto& compTrans = _context.registry().get<CompTransform>(word);
            compTrans.position.y = y;

            auto& compFightText = _context.registry().get<CompFightText>(word);
            compFightText.color.a = (uint8_t)a;

            return false;
        });


    }
}