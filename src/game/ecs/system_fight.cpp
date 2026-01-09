#include "system_fight.h"



namespace game 
{
	FightSystem::FightSystem(GameContext& context) : EcsSystem(context) 
	{
		_context.dispatcher().sink<CastSkillToObject>().connect<&FightSystem::onCastSkillToObject>(this);
		_context.dispatcher().sink<RoleOnAttack>().connect<&FightSystem::onRoleUnderAttack>(this);
		_context.dispatcher().sink<ProjectileHitPos>().connect<&FightSystem::onProjectileHitPos>(this);
		_context.dispatcher().sink<ExecSkillEvent>().connect<&FightSystem::onSkillEvent>(this);
		
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

	tweeny::tween<float, float> FightSystem::makeSkillTween(const CastSkillToObject& e)
	{
		if (!_context.registry().valid(e.source))
		{
			return tweeny::tween<float, float>{};
		}

		auto& srcTrans = _context.registry().get<CompTransform>(e.source);
		auto& skillAffect = _context.registry().get<CompSkillAffect>(e.skill);
		auto& skillTween = _context.registry().get<CompSkillTween>(e.skill);
		auto& transValue = skillTween.trans_value;

		if(skillTween.trans_type == TweenTransform::Motion)
		{
			auto& srcPos = srcTrans.position;
			Vec2 tgtPos = { 0, 0 };
			if (_context.registry().valid(e.target))
			{
				auto& tgtTrans = _context.registry().get<CompTransform>(e.target);
				tgtPos = tgtTrans.position;
			}

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

		return tweeny::tween<float, float>{};
	}

	void FightSystem::onCastSkillToObject(const CastSkillToObject& e)
	{
		spdlog::info("CastSkillToObject: source ({}) -> target ({})", (uint32_t)e.source, (uint32_t)e.target );

		if (_context.registry().valid(e.skill) == false)
		{
			spdlog::warn("onCastSkillToObject: skill ({}) is invalid", (uint32_t)e.skill);
			return;
		}

		auto& compName = _context.registry().get<CompNameId>(e.skill);
		auto& skillComm = _context.registry().get<CompSkillComm>(e.skill);
		if (skillComm.state != SkillState::OK)
		{
			spdlog::warn("onCastSkillToObject: skill ({}) state is NOT OK", compName.cfg_id);
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
			}
			return false;
		});
	}

	void FightSystem::skillAffectApplyToObject(const CastSkillToObject& e)
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

	void FightSystem::startProjectileObject(entt::entity srcid, entt::entity tarid, entt::entity skill)
	{
		auto projectComp = _context.registry().try_get<CompProjectileCfg>(skill);
		if (!projectComp)
		{
			return;
		}

		auto& compProjectile = *projectComp;
		auto& compSrcTrans = _context.registry().get<CompTransform>(srcid);
		auto& compTgtTrans = _context.registry().get<CompTransform>(tarid);

		const auto& tweentype = compProjectile.tween;
		const auto& source = compSrcTrans.position;
		const auto& target = compTgtTrans.position;
		float speed = compProjectile.speed == 0? 100 : compProjectile.speed;
		int during = static_cast<int>((glm::distance(source, target) / speed) * 1000);

		auto object = _context.objectFactory().createProjectile(source, target, speed, compProjectile.tween, compProjectile.particle);
		if (!_context.registry().valid(object))
		{
			return;
		}

		auto& compTrans = _context.registry().get<CompTransform>(object);
		compTrans.position = source;
		compTrans.size = {10, 10};
		compTrans.rotation = {0, 0};
		compTrans.scale = { 1, 1 };

		CompShoot compShoot;
		compShoot.tween = tweeny::from(source.x, source.y)
			.to(target.x, target.y)
			.via(tweentype)
			.during(during)
			.to(target.x, target.y)
			.via(tweentype)
			.during(200);

		// 生效，等200ms再销毁，立刻摧毁显得效果僵硬
		compShoot.tween.onPoint([this, srcid, skill, target](auto& t, float x, float y) {
			ProjectileHitPos e;
			e.source = srcid;
			e.skill = skill;
			e.pos = target;
			_context.dispatcher().trigger(e);
			return false;
		});

		compShoot.tween.onStep([this, object](auto& t, float x, float y) {
			if (!_context.registry().valid(object)) {
				return false;
			}

			if (t.isFinished()) {
				_context.registry().emplace<CompDestroy>(object);
				return true;
			}

			auto& compTrans = _context.registry().get<CompTransform>(object);
			compTrans.position = { x, y };
			return false;
		});
		_context.registry().emplace<CompShoot>(object, compShoot);
	}

	void FightSystem::onRoleUnderAttack(const RoleOnAttack& e)
	{
		auto underatk = _context.registry().try_get<CompUnderAttack>(e.target);
		if(!underatk)
		{
			return;
		}

		auto& targetNameComp = _context.registry().get<CompNameId>(e.target);
		auto& skillNameComp = _context.registry().get<CompNameId>(e.skill);
		spdlog::info("object: id({}), cfg({}), name({}) On Attack !!! skill.cfg({}), skill.name({})", 
			(uint32_t)targetNameComp.id, targetNameComp.cfg_id, targetNameComp.name,
			skillNameComp.cfg_id, skillNameComp.name);

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
		spdlog::info("projectile: source({}) skill({}) hit ({},{})", 
			(uint32_t)e.source, (uint32_t)e.skill, e.pos.x, e.pos.y);

		auto& compAffect = _context.registry().get<CompSkillAffect>(e.skill);
		auto range = compAffect.affect_range;

		auto objects = _context.currentScene().getObjectsInCircle(e.pos, range);
		for (auto& [d,obj] : objects) 
		{
			auto& cmpComm = _context.registry().get<CompComm>(obj);
			if (cmpComm.type == ObjectType::Npc)
			{
				_context.dispatcher().trigger(RoleOnAttack{ e.source, obj, e.skill });
			}
		}
	}

	void FightSystem::onSkillEvent(const ExecSkillEvent& e)
	{
		spdlog::info("onSkillEvent: event = source({}), skill({}), event({})", 
			(uint32_t)e.source, (uint32_t)e.skill, e.event);

		const auto& views = utility::StringUtil::split(e.event, ',');
		if (views.size() < 1) return;

		if (views[0] == "sky_turn_dark")
		{
			assert(views.size() == 2);

			int ticks = std::stoi(views[1].data());
			if (ticks < 2000) {
				spdlog::error("sky_turn_dar time must greater than 2000");
				ticks = 2000;
			}

			_context.objectFactory().createSkyEffect(SkyEffect::Dark, Color{ 0,0,0,200 }, ticks-1000, 500, 500);
		}
	}

}