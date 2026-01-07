#include "system_fight.h"



namespace game 
{
	FightSystem::FightSystem(GameContext& context) : EcsSystem(context) 
	{
		_context.dispatcher().sink<RoleExecSkillToObject>().connect<&FightSystem::onRoleExecSkillToObject>(this);
		_context.dispatcher().sink<RoleOnAttack>().connect<&FightSystem::onRoleUnderAttack>(this);
		_context.dispatcher().sink<ProjectileHitPos>().connect<&FightSystem::onProjectileHitPos>(this);
	}
	
	FightSystem::~FightSystem() 
	{
	}

	void FightSystem::update(float delta)
	{
		auto deltaTicks = _context.frameTicker().deltaTicks();

		// 受击特效
		auto views1 = _context.registry().view<CompUnderAttack>();
		for(auto& ent : views1)
		{
			auto underATK = _context.registry().try_get<CompUnderAttack>(ent);
			if(underATK && underATK->under_attack)
			{
				underATK->tween.step(deltaTicks);
			}
		}

		auto views = _context.registry().view<CompNameId, CompSkillComm>();
		for (auto& ent : views)
		{
			// 技能冷却
			auto& skillComm = _context.registry().get<CompSkillComm>(ent);
			if (skillComm.state == SkillState::Cooling)
			{
				auto skillCD = _context.registry().try_get<CompSkillCD>(ent);
				if (skillCD)
				{
					skillCD->current_tick += deltaTicks;
					if (skillCD->current_tick >= skillCD->total_ticks) 
					{
						skillCD->current_tick = 0;
						skillComm.state = SkillState::OK;
					}
				}
			}

			// 技能特效
			if (skillComm.state == SkillState::Launching)
			{
				auto skillTween = _context.registry().try_get<CompSkillTween>(ent);
				if (skillTween)
				{
					skillTween->tween.step(deltaTicks);
				}
			}
		}
	}

	tweeny::tween<float, float> FightSystem::makeSkillTween(const RoleExecSkillToObject& e)
	{
		auto& srcTrans = _context.registry().get<CompTransform>(e.source);
		auto& tgtTrans = _context.registry().get<CompTransform>(e.target);

		auto& skillAffect = _context.registry().get<CompSkillAffect>(e.skill);
		auto& skillTween = _context.registry().get<CompSkillTween>(e.skill);
		auto& transValue = skillTween.trans_value;

		if(skillTween.trans_type == TweenTransform::Motion)
		{
			auto& srcPos = srcTrans.position;
			auto& tgtPos = tgtTrans.position;
			auto dstPos = srcPos + glm::normalize(tgtPos - srcPos)* transValue;

			return tweeny::from(srcPos.x, srcPos.y)
				.to(dstPos.x, dstPos.y)
				.via(skillTween.prev_tween)
				.during(skillAffect.prev_ticks)
				.to(srcPos.x, srcPos.y)
				.via(skillTween.post_tween)
				.during(skillAffect.post_ticks)
				.onStep([e, this](auto& t, float x, float y) {
						if (_context.registry().valid(e.source)) {
							auto& srcTrans = _context.registry().get<CompTransform>(e.source);
							srcTrans.position = { x, y };
						}
						return false;
					});
		} 
		
		if(skillTween.trans_type == TweenTransform::Scale)
		{
			auto& srcSize = srcTrans.size;
			//auto& tgtSize = tgtTrans.size;
			auto transValue = skillTween.trans_value;
			auto dstSize = srcSize + Vec2{transValue, transValue};

			return tweeny::from(srcSize.x, srcSize.y)
				.to(dstSize.x, dstSize.y)
				.via(skillTween.prev_tween)
				.during(skillAffect.prev_ticks)
				.to(srcSize.x, srcSize.y)
				.via(skillTween.post_tween)
				.during(skillAffect.post_ticks)
				.onStep([e, this](auto& t, float x, float y) {
						if (_context.registry().valid(e.source)) {
							auto& srcTrans = _context.registry().get<CompTransform>(e.source);
							srcTrans.size = { x, y };
						}
						return false;
					});
		}

		tweeny::tween<float, float> tween;
		return tween;
	}

	void FightSystem::onRoleExecSkillToObject(const RoleExecSkillToObject& e)
	{
		spdlog::info("RoleExecSkillToObject: source ({}) -> target ({})", (int)e.source, (int)e.target );

		if (_context.registry().valid(e.skill) == false)
		{
			spdlog::warn("onRoleExecSkillToObject: skill ({}) is invalid", (int)e.skill);
			return;
		}

		auto& compName = _context.registry().get<CompNameId>(e.skill);
		auto& skillComm = _context.registry().get<CompSkillComm>(e.skill);
		if (skillComm.state != SkillState::OK)
		{
			spdlog::warn("onRoleExecSkillToObject: skill ({}) state is NOT OK", compName.cfg_id);
			return;
		}

		skillComm.state = SkillState::Launching;

		auto& skillTween = _context.registry().get<CompSkillTween>(e.skill);

		skillTween.tween = makeSkillTween(e);

		skillTween.tween.onPoint([e, this](auto& t, float x, float y) {
				if (_context.registry().valid(e.source) == false) {
					return false;
				}

				skillAffectApplyToObject(e);
				return false;
			});

		skillTween.tween.onStep([e, this](auto& t, float x, float y) {
			if (t.isFinished()) {
				auto& skillComm = _context.registry().get<CompSkillComm>(e.skill);
				skillComm.state = SkillState::Cooling;
				return false;
			}
		});
	}

	void FightSystem::skillAffectApplyToObject(const RoleExecSkillToObject& e)
	{
		auto& compName = _context.registry().get<CompNameId>(e.skill);
		auto& skillComm = _context.registry().get<CompSkillComm>(e.skill);

		spdlog::info("skill id:{} cfg:{} affect !", (uint32_t)compName.id, compName.cfg_id);

		if (skillComm.type == SkillType::Combat)
		{
			_context.dispatcher().trigger(RoleOnAttack{e.source, e.target, e.skill});
		}
		else if (skillComm.type == SkillType::Projectile)
		{
			startProjectileObject(e.source, e.target, e.skill);
		}
		else
		{
			auto& skillAffect = _context.registry().get<CompSkillAffect>(e.skill);
			_context.dispatcher().trigger(ExecSkillEvent{e.source, e.skill, skillAffect.event});
		}
	}

	void FightSystem::startProjectileObject(entt::entity source, entt::entity target, entt::entity skill)
	{
		auto projectComp = _context.registry().try_get<CompProjectileCfg>(skill);
		if (!projectComp)
		{
			return;
		}

		auto& compProjectile = *projectComp;
		auto& compSrcTrans = _context.registry().get<CompTransform>(source);
		auto& compTgtTrans = _context.registry().get<CompTransform>(target);

		const auto& srcPos = compSrcTrans.position;
		const auto& dstPos = compTgtTrans.position;
		float speed = compProjectile.speed == 0? 100 : compProjectile.speed;

		_context.objectFactory().createProjectile(srcPos, dstPos, speed, compProjectile.tween, compProjectile.particle);
	}

	void FightSystem::onRoleUnderAttack(const RoleOnAttack& e)
	{
		auto underatk = _context.registry().try_get<CompUnderAttack>(e.target);
		if(!underatk)
		{
			return;
		}

		auto& srcTrans = _context.registry().get<CompTransform>(e.source);
		auto& dstTrans = _context.registry().get<CompTransform>(e.target);
		auto& skillAffect = _context.registry().get<CompSkillAffect>(e.skill);

		const auto& srcPos = srcTrans.position;
		const auto& rolePos = dstTrans.position;

		auto offset = glm::normalize(rolePos - srcPos)* underatk->motion_offset;
		auto dstPos = rolePos + offset;

		underatk->under_attack = true;
		underatk->tween = tweeny::from(rolePos.x, rolePos.y)

			.to(dstPos.x, dstPos.y)
			.via(underatk->prev_tween)
			.during(underatk->during / 2)

			.to(rolePos.x, rolePos.y)
			.via(underatk->post_tween)
			.during(underatk->during / 2)

			.onStep([e, this](auto& t, float x, float y) 
			{
				if (_context.registry().valid(e.target)) 
				{
					auto& dstTrans = _context.registry().get<CompTransform>(e.target);
					dstTrans.position = { x, y };

					if(t.isFinished())
					{
						auto underatk = _context.registry().try_get<CompUnderAttack>(e.target);
						if(underatk) 
						{
							underatk->under_attack = false;
						}
					}
				}
				return false;
			});
	}

	void FightSystem::onProjectileHitPos(const ProjectileHitPos& e)
	{
		spdlog::info("projectile: {} hit at ({},{})", (int)e.projectile, e.pos.x, e.pos.y);
	}

}