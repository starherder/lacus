#include "system_skill.h"



namespace game 
{
	CompSprint g_testSprint;


	SkillSystem::SkillSystem(GameContext& context) : EcsSystem(context) 
	{
		_context.dispatcher().sink<EvtCastSkillToObject>().connect<&SkillSystem::onCastSkillToObject>(this);
		_context.dispatcher().sink<EvtRoleOnAttack>().connect<&SkillSystem::onRoleUnderAttackEffect>(this);
		_context.dispatcher().sink<EvtProjectileHitPos>().connect<&SkillSystem::onProjectileHitPos>(this);
		_context.dispatcher().sink<EvtExecSkillEvent>().connect<&SkillSystem::onSkillEvent>(this);
	}
	
	SkillSystem::~SkillSystem() 
	{
	}

	void SkillSystem::update(float delta)
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
			auto& skillComm = views.get<CompSkillComm>(ent);
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

			// 施法特效
			if (skillComm.state == SkillState::Launching)
			{
				auto skillTween = _context.registry().try_get<CompSkillSpell>(ent);
				if (skillTween)
				{
					skillTween->tween.step(deltaTicks);
				}
			}
		}

		// 冲刺
		auto viewsSprint = _context.registry().view<CompSprint>();
		for (auto& ent : viewsSprint)
		{
			auto& sprint = viewsSprint.get<CompSprint>(ent);
			if (sprint.running) 
			{
				sprint.tween.step(deltaTicks);
			}
		}
	}

	tweeny::tween<float, float> SkillSystem::makeSkillSpellTween(const EvtCastSkillToObject& e)
	{
		if (!_context.registry().valid(e.source))
		{
			return tweeny::tween<float, float>{};
		}

		auto& srcTrans = _context.registry().get<CompTransform>(e.source);
		auto& skillAffect = _context.registry().get<CompSkillAffect>(e.skill);
		auto& skillTween = _context.registry().get<CompSkillSpell>(e.skill);
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

			auto dstPos = srcPos + SafeNormal(tgtPos-srcPos)* transValue;

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

	void SkillSystem::onCastSkillToObject(const EvtCastSkillToObject& e)
	{
		//SPDLOG_INFO("EvtCastSkillToObject: source ({}) -> target ({})", (uint32_t)e.source, (uint32_t)e.target );

		if (_context.registry().valid(e.skill) == false)
		{
			SPDLOG_WARN("onCastSkillToObject: skill ({}) is invalid", (uint32_t)e.skill);
			return;
		}

		auto& compName = _context.registry().get<CompNameId>(e.skill);
		auto& skillComm = _context.registry().get<CompSkillComm>(e.skill);
		if (skillComm.state != SkillState::OK)
		{
			SPDLOG_WARN("onCastSkillToObject: skill ({}) state is NOT OK", compName.cfg_id);
			return;
		}

		skillComm.state = SkillState::Launching;

		auto& skillTween = _context.registry().get<CompSkillSpell>(e.skill);
		skillTween.tween = makeSkillSpellTween(e);
		skillTween.tween.onPoint([e, this](auto& t, float x, float y) {
				if (_context.registry().valid(e.source) == false || t.isFinished()) {
					return true;
				}

				skillApplyToTarget(e);
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

	void SkillSystem::skillApplyToTarget(const EvtCastSkillToObject& e)
	{
		auto& compName = _context.registry().get<CompNameId>(e.skill);
		auto& skillComm = _context.registry().get<CompSkillComm>(e.skill);

		auto pcompAudio = _context.registry().try_get<CompAudio>(e.skill);
		if(pcompAudio && pcompAudio->audio_name.size() > 0)
		{
			_context.audioPlayer().playSound(HashString(pcompAudio->audio_name.c_str()));
		}

		//SPDLOG_INFO("skill id:{} cfg:{} affect !", (uint32_t)compName.id, compName.cfg_id);

		if (skillComm.type == SkillType::Combat)
		{
			_context.dispatcher().trigger(EvtRoleOnAttack{e.source, e.target, e.skill});
		}
		else if (skillComm.type == SkillType::Projectile)
		{
			startProjectile(e.source, e.target, e.skill);
		}
		else if (skillComm.type == SkillType::Trap)
		{
			startTraps(e.source, e.target, e.skill);
		}
		else if (skillComm.type == SkillType::Sprint)
		{
			startSprint(e.source, e.target, e.skill);
		}
		else
		{
			auto& skillAffect = _context.registry().get<CompSkillAffect>(e.skill);
			_context.dispatcher().trigger(EvtExecSkillEvent{e.source, e.skill, skillAffect.event});
		}
	}

	void SkillSystem::startSprint(entt::entity srcid, entt::entity tarid, entt::entity skill)
	{
		auto pSprintComp = _context.registry().try_get<CompSprint>(skill);
		if (!pSprintComp)
		{
			return;
		}

		auto& sprintComp = *pSprintComp;
		assert(sprintComp.speed != 0.0f);

		auto& compSrcTrans = _context.registry().get<CompTransform>(srcid);
		auto& compDstTrans = _context.registry().get<CompTransform>(tarid);

		auto& srcPos = compSrcTrans.position;
		auto dstPos = srcPos + SafeNormal(compDstTrans.position - srcPos) * sprintComp.dis;
		auto during = (int)(1000 * glm::distance(dstPos, srcPos) / sprintComp.speed);

		sprintComp.running = true;
		sprintComp.tween = tweeny::from(srcPos.x, srcPos.y)
								.to(dstPos.x, dstPos.y)
								.via(sprintComp.tween_mode)
								.during(during).onStep(
									[this, srcid, skill](auto& t, float x, float y) {

										auto pSrcSprint = _context.registry().try_get<CompSprint>(skill);
										auto pSrcTrans = _context.registry().try_get<CompTransform>(srcid);
										auto pSrcComm = _context.registry().try_get<CompComm>(srcid);
										if (!pSrcSprint || !pSrcTrans || !pSrcComm) 
										{
											SPDLOG_ERROR("srcid ({}) transform or comm component not exist.", (uint64_t)srcid);
											return true;
										}

										if (t.isFinished()) {
											pSrcSprint->running = false;
											return true;
										}

										auto grid = _context.scene().getGridFromPos(Vec2{x, y});
										if (_context.scene().getGridWalkType(grid) == (int)tilemap::WalkType::Collision) 
										{
											pSrcSprint->running = false;
											SPDLOG_ERROR("next grid({}, {}) is collision.", grid.x, grid.y);
											return true;
										}

										pSrcTrans->position = Vec2{x, y};

										auto sprintComp = _context.registry().get<CompSprint>(skill);
										auto it = sprintComp.passed_grids.find(grid);
										if (it == sprintComp.passed_grids.end())
										{
											auto objects = _context.scene().getObjectsInGrid(grid);
											for (auto& obj : objects)
											{
												auto pObjComm = _context.registry().try_get<CompComm>(obj);
												if (pObjComm && pObjComm->type == ObjectType::Npc && pObjComm->side != pSrcComm->side)
												{
													_context.dispatcher().trigger(EvtRoleOnAttack{ srcid, obj, skill });
												}
											}

											sprintComp.passed_grids.insert(grid);
										}

										return false;
									}
								);
	}

	void SkillSystem::startTraps(entt::entity srcid, entt::entity tarid, entt::entity skill)
	{
		auto pTrapsComp = _context.registry().try_get<CompTraps>(skill);
		if (!pTrapsComp)
		{
			return;
		}

		auto& trapsComp = *pTrapsComp;
		auto& compSrcTrans = _context.registry().get<CompTransform>(srcid);
		auto& compTgtTrans = _context.registry().get<CompTransform>(tarid);

	}

	void SkillSystem::startProjectile(entt::entity srcid, entt::entity tarid, entt::entity skill)
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

		_context.registry().emplace<CompShoot>(object);

		auto& compShoot = _context.registry().get<CompShoot>(object);
		compShoot.tween = tweeny::from(source.x, source.y)
			.to(target.x, target.y)
			.via(tweentype)
			.during(during)
			.to(target.x, target.y)
			.via(tweentype)
			.during(200);

		// 生效，等200ms再销毁，立刻摧毁显得效果僵硬
		compShoot.tween.onPoint([this, srcid, skill, target](auto& t, float x, float y) {
			if (t.isFinished()) {
				return true;
			}

			EvtProjectileHitPos e;
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
				_context.registry().emplace_or_replace<CompDestroy>(object);
				return true;
			}

			auto& compTrans = _context.registry().get<CompTransform>(object);
			compTrans.position = { x, y };
			return false;
		});
	}

	void SkillSystem::onProjectileHitPos(const EvtProjectileHitPos& e)
	{
		//SPDLOG_INFO("projectile: source({}) skill({}) hit ({},{})", 
		//	(uint32_t)e.source, (uint32_t)e.skill, e.pos.x, e.pos.y);

		auto& compAffect = _context.registry().get<CompSkillAffect>(e.skill);
		auto range = compAffect.range;

		auto objects = _context.scene().getObjectsInCircle(e.pos, range);
		for (auto& [d,obj] : objects) 
		{
			auto pCmpComm = _context.registry().try_get<CompComm>(obj);
			if (pCmpComm && pCmpComm->type == ObjectType::Npc)
			{
				_context.dispatcher().trigger(EvtRoleOnAttack{ e.source, obj, e.skill });
			}
		}
	}

	void SkillSystem::onSkillEvent(const EvtExecSkillEvent& e)
	{
		SPDLOG_INFO("onSkillEvent: event = source({}), skill({}), event({})", 
			(uint32_t)e.source, (uint32_t)e.skill, e.event);

		const auto& views = utility::StringUtil::split(e.event, ',');
		if (views.size() < 1) return;

		if (views[0] == "sky_turn_dark")
		{
			assert(views.size() == 2);

			int ticks = std::stoi(views[1].data());
			if (ticks < 2000) {
				SPDLOG_ERROR("sky_turn_dar time must greater than 2000");
				ticks = 2000;
			}

			_context.objectFactory().createSkyEffect(SkyEffect::Dark, Color{ 0,0,0,200 }, ticks-1000, 500, 500);
		}
	}

	void SkillSystem::onRoleUnderAttackEffect(const EvtRoleOnAttack& e)
	{
		auto underatk = _context.registry().try_get<CompUnderAttack>(e.target);
		if(!underatk)
		{
			return;
		}

		auto& targetNameComp = _context.registry().get<CompNameId>(e.target);
		auto& skillNameComp = _context.registry().get<CompNameId>(e.skill);

		//SPDLOG_INFO("object: id({}), cfg({}), name({}) On Attack !!! skill.cfg({}), skill.name({})", 
		//	(uint32_t)targetNameComp.id, targetNameComp.cfg_id, targetNameComp.name, skillNameComp.cfg_id, skillNameComp.name);

		auto& srcTrans = _context.registry().get<CompTransform>(e.source);
		auto& dstTrans = _context.registry().get<CompTransform>(e.target);
		auto& skillAffect = _context.registry().get<CompSkillAffect>(e.skill);

		const auto& srcPos = srcTrans.position;
		const auto& rolePos = dstTrans.position;

		auto offset = SafeNormal(rolePos - srcPos) * underatk->motion_offset;
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

}