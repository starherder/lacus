#include "system_skill.h"

#include <array>

namespace game 
{

	DeclareEcsSystem(SkillSystem, EcsPriority::Middle);


	SkillSystem::SkillSystem(GameContext& context) : EcsSystem(context) 
	{
		_context.dispatcher().sink<EvtCastSkillToObject>().connect<&SkillSystem::onCastSkillToObject>(this);
		_context.dispatcher().sink<EvtCastSkillToPos>().connect<&SkillSystem::onCastSkillToPos>(this);
		_context.dispatcher().sink<EvtRoleOnAttack>().connect<&SkillSystem::onRoleUnderAttackEffect>(this);
		_context.dispatcher().sink<EvtSkillHitPos>().connect<&SkillSystem::onSkillHitPos>(this);
		_context.dispatcher().sink<EvtSkillHitTarget>().connect<&SkillSystem::onSkillHitTarget>(this);
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

		// 陷阱
		auto viewsTrap = _context.registry().view<CompTraps>();
		for (auto& ent : viewsTrap)
		{
			auto& trap = viewsTrap.get<CompTraps>(ent);
			if (trap.onUpdate)
			{
				trap.onUpdate(deltaTicks);
			}
		}

		// 冲击波
		auto viewsWave = _context.registry().view<CompWave>();
		for (auto& ent : viewsWave)
		{
			auto& wave = viewsWave.get<CompWave>(ent);
			if (wave.onUpdate)
			{
				wave.onUpdate(deltaTicks);
			}
		}

		// 闪电
		auto viewsLightning = _context.registry().view<CompLightning>();
		for (auto& ent : viewsLightning)
		{
			auto& lightning = viewsLightning.get<CompLightning>(ent);
			if (lightning.onUpdate)
			{
				lightning.onUpdate(deltaTicks);
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
							//srcTrans.position = { x, y };
							_context.scene().setObjectPos(e.source, { x, y });
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
							//_context.scene().setObjectPos(e.source, srcTrans.position);
						}
						return false;
					});
		}

		return tweeny::tween<float, float>{};
	}

	void SkillSystem::onCastSkillToPos(const EvtCastSkillToPos& e)
	{
		if (_context.registry().valid(e.skill) == false)
		{
			LogWarn("skill ({}) is invalid", (uint32_t)e.skill);
			return;
		}

		auto& compName = _context.registry().get<CompNameId>(e.skill);
		auto& skillComm = _context.registry().get<CompSkillComm>(e.skill);
		if (skillComm.state != SkillState::OK)
		{
			LogWarn("skill ({}) state is NOT OK", compName.cfg_id);
			return;
		}

		// TODO: cast skill to pos
	}

	void SkillSystem::onCastSkillToObject(const EvtCastSkillToObject& e)
	{
		if(!_context.registry().valid(e.target))
		{
			return;
		}

		auto pCompDead = _context.registry().try_get<CompDead>(e.target);
		if(pCompDead)
		{
			return;
		}

		if (_context.registry().valid(e.skill) == false)
		{
			LogWarn("skill ({}) is invalid", (uint32_t)e.skill);
			return;
		}

		auto& compName = _context.registry().get<CompNameId>(e.skill);
		auto& skillComm = _context.registry().get<CompSkillComm>(e.skill);
		if (skillComm.state != SkillState::OK)
		{
			LogWarn("skill ({}) state is NOT OK", compName.cfg_id);
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
		else if (skillComm.type == SkillType::Wave)
		{
			startWave(e.source, e.target, e.skill);
		}
		else if (skillComm.type == SkillType::Lightning)
		{
			startLightning(e.source, e.target, e.skill);
		}
		else
		{
			auto& skillAffect = _context.registry().get<CompSkillAffect>(e.skill);
			_context.dispatcher().trigger(EvtExecSkillEvent{e.source, e.skill, skillAffect.event});
		}
	}

	void SkillSystem::startWave(entt::entity srcid, entt::entity tarid, entt::entity skill)
	{
		auto pWaveCfg = _context.registry().try_get<CompWaveCfg>(skill);
		if (!pWaveCfg)
		{
			return;
		}

		auto& compSrcTrans = _context.registry().get<CompTransform>(srcid);
		auto& compDstTrans = _context.registry().get<CompTransform>(tarid);

		auto wave = _context.registry().create();
		_context.registry().emplace<CompWave>(wave);

		auto& compWave = _context.registry().get<CompWave>(wave);
		compWave.wave_ticks = 0;
		compWave.cur_wave = 0;
		compWave.onUpdate = [this, srcid, tarid, skill, wave](int64_t tick) 
		{
			auto& waveCfg = _context.registry().get<CompWaveCfg>(skill);
			auto& compWave = _context.registry().get<CompWave>(wave);

			if (compWave.wave_ticks == 0)
			{
				createWaveRange(compWave.cur_wave, srcid, tarid, skill);
				compWave.cur_wave++;
			}
			else if (compWave.wave_ticks >= waveCfg.interval)
			{
				compWave.wave_ticks = 0;
				return;
			}
			
			if (compWave.cur_wave >= waveCfg.grids)
			{
				_context.scene().destroyObject(wave);
				return;
			}

			compWave.wave_ticks += (int)tick;
		};
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
											LogError("srcid ({}) transform or comm component not exist.", (uint64_t)srcid);
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
											LogError("next grid({}, {}) is collision.", grid.x, grid.y);
											return true;
										}

										//pSrcTrans->position = Vec2{x, y};
										_context.scene().setObjectPos(srcid, { x,y });

										auto sprintComp = _context.registry().get<CompSprint>(skill);
										auto it = sprintComp.passed_grids.find(grid);
										if (it == sprintComp.passed_grids.end())
										{
											const auto& objects = _context.scene().getObjectsInGrid(grid);
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
		auto& compTgtTrans = _context.registry().get<CompTransform>(tarid);
		createTrapAtPos(srcid, compTgtTrans.position, skill, ShapeType::Circle);
	}

	void SkillSystem::startLightning(entt::entity srcid, entt::entity tarid, entt::entity skill)
	{
		auto pCompLightningCfg = _context.registry().try_get<CompLightningCfg>(skill);
		auto pCompAffect = _context.registry().try_get<CompSkillAffect>(skill);
		if (!pCompAffect || !pCompLightningCfg)
		{
			return;
		}

		float atk_dis = pCompAffect->range;
		int tgt_count = pCompLightningCfg->target_count;

		auto& srcTrans = _context.registry().get<CompTransform>(srcid);
		auto& srcComm = _context.registry().get<CompComm>(srcid);
		auto& objects = _context.scene().getObjectsInCircle(srcTrans.position, atk_dis);
		tgt_count = tgt_count > (int)objects.size() ? (int)objects.size() : tgt_count;

		int index = 0;
		for (auto& [dis, obj] : objects)
		{
			if(obj == srcid || !_context.registry().valid(obj)) 
			{
				continue;
			}

			auto objComm = _context.registry().try_get<CompComm>(obj);
			if(!objComm || objComm->type != ObjectType::Npc || objComm->side == srcComm.side)
			{
				continue;
			}

			if (index++ >= tgt_count)
			{
				break;
			}

			createLightningToTarget(srcid, obj, skill);
		}
	}

	void SkillSystem::createWaveRange(int r, entt::entity srcid, entt::entity tarid, entt::entity skill)
	{
		auto& waveCfg = _context.registry().get<CompWaveCfg>(skill);
		auto& compSrcTrans = _context.registry().get<CompTransform>(srcid);
		auto& compDstTrans = _context.registry().get<CompTransform>(tarid);

		auto normlToCoord = [](const Vec2& dir) {
			float x = std::fabs(dir.x) > std::fabs(dir.y) ? dir.x : 0;
			float y = std::fabs(dir.x) > std::fabs(dir.y) ? 0 : dir.y;
			return SafeNormal({x, y});
		};

		if (waveCfg.type == 1)
		{
			auto dir = normlToCoord(compDstTrans.position - compSrcTrans.position);
			auto trapPos = compSrcTrans.position + _context.scene().tileSize() * dir * (float)(r+1);
			createTrapAtPos(srcid, trapPos, skill, ShapeType::Rect);
		}
		else if (waveCfg.type == 2)
		{
			std::vector<Vec2> targets = { {0, -1}, {0, 1}, {-1, 0}, {1, 0} };
			for (auto& dir : targets)
			{
				auto pos = compSrcTrans.position + _context.scene().tileSize() * dir * (float)(r+1);
				createTrapAtPos(srcid, pos, skill, ShapeType::Rect);
			}
		}
		else if (waveCfg.type == 3)
		{
			int min_r = -r - 1, max_r = r + 1;
			for (int x = min_r; x <= max_r; x++)
			{
				for (int y = min_r; y <= max_r; y++)
				{
					if (x == min_r || x == max_r || y == min_r || y == max_r)
					{
						auto pos = compSrcTrans.position + _context.scene().tileSize() * Vec2 { x, y };
						createTrapAtPos(srcid, pos, skill, ShapeType::Rect);
					}
				}
			}
		}
		else
		{
			LogError("wavecfg.type {} NOT support", waveCfg.type);
		}
	}

	void SkillSystem::createLightningToTarget(entt::entity source, entt::entity target, entt::entity skill)
	{
		auto pCompLightningCfg = _context.registry().try_get<CompLightningCfg>(skill);
		if(!pCompLightningCfg)
		{
			return;
		}

		auto& srcTrans = _context.registry().get<CompTransform>(source);
		auto& dstTrans = _context.registry().get<CompTransform>(target);

		// create a lightning
		auto lightning = _context.registry().create();
		_context.registry().emplace<CompLightning>(lightning);

		//LogInfo("create light: {}", lightning);

		// set lightning
		auto& compLightning = _context.registry().get<CompLightning>(lightning);
		compLightning.atk_ticks = 0;
		compLightning.cur_atk = 0;
		compLightning.onUpdate = [this, source, target, skill, lightning](int64_t tick)
			{
				auto& lightningCfg = _context.registry().get<CompLightningCfg>(skill);
				auto& compLightning = _context.registry().get<CompLightning>(lightning);
				auto& compDisplay = _context.registry().get<CompLightningDisplay>(lightning);

				if (compLightning.atk_ticks == 0)
				{
					EvtSkillHitTarget e;
					e.source = source;
					e.skill = skill;
					e.target = target;
					_context.dispatcher().trigger(e);

					compDisplay.color = lightningCfg.color;
					compLightning.cur_atk++;
				}
				else if (compLightning.atk_ticks >= lightningCfg.interval)
				{
					compLightning.atk_ticks = 0;

					if (compLightning.cur_atk >= lightningCfg.attack_times)
					{
						_context.scene().destroyObject(lightning);
					}

					return;
				}

				compDisplay.color. a = (int)(255*(1.0f - (float)compLightning.atk_ticks/(float)lightningCfg.during));
				compLightning.atk_ticks += (int)tick;
			};

		// add display
		CompLightningDisplay disp;
		disp.color = pCompLightningCfg->color;
		disp.during = pCompLightningCfg->during;
		disp.thickness = pCompLightningCfg->thickness;
		_context.painter().makeLightningData(disp.data, srcTrans.position, dstTrans.position, pCompLightningCfg->displace);
		
		_context.registry().emplace_or_replace<CompLightningDisplay>(lightning, disp);
	}

	entt::entity SkillSystem::createTrapAtPos(entt::entity srcid, const Vec2& target, entt::entity skill, ShapeType shape_type)
	{
		auto pTrapCfgComp = _context.registry().try_get<CompTrapCfg>(skill);
		if (!pTrapCfgComp)
		{
			return entt::null;
		}

		auto pos = _context.scene().normalToGridPos(target);
		auto trap = _context.objectFactory().createTrap(pos, pTrapCfgComp->range,
														pTrapCfgComp->color, pTrapCfgComp->texture,
														pTrapCfgComp->particle, shape_type);
		
		int src_alpha = pTrapCfgComp->color.a;

		auto& compTrap = _context.registry().get<CompTraps>(trap);
		compTrap.during_ticks = 0;
		compTrap.period_ticks = 0;
		compTrap.onUpdate = [this, srcid, skill, trap, src_alpha](int64_t ticks)
		{
			if (!_context.registry().valid(skill))
			{
				return;
			}

			auto& compTraps = _context.registry().get<CompTraps>(trap);
			auto& compTrapCfg = _context.registry().get<CompTrapCfg>(skill);

			if (compTrapCfg.duration > 0 && compTraps.during_ticks >= compTrapCfg.duration)
			{
				_context.scene().destroyObject(trap);
				return;
			}

			if (compTraps.period_ticks == 0)
			{
				onTrapPeriodExec(srcid, skill, trap);
			}
			else if (compTraps.period_ticks > compTrapCfg.period)
			{
				compTraps.period_ticks = 0;
				return;
			}

			float period_ratio = 1.0f - (float)compTraps.period_ticks / (float)compTrapCfg.period;
			auto& compDisplay = _context.registry().get<CompMarkDisplay>(trap);
			compDisplay.ground_color.a = (int)(src_alpha * period_ratio);

			compTraps.period_ticks += (int)ticks;
			compTraps.during_ticks += (int)ticks;
		};

		return trap;
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

		auto projectile = _context.objectFactory().createProjectile(source, target, compProjectile.particle);
		if (!_context.registry().valid(projectile))
		{
			return;
		}

		auto& compTrans = _context.registry().get<CompTransform>(projectile);
		compTrans.position = source;
		compTrans.size = {10, 10};
		compTrans.rotation = {0, 0};
		compTrans.scale = { 1, 1 };

		_context.registry().emplace<CompShoot>(projectile);

		auto& compShoot = _context.registry().get<CompShoot>(projectile);
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

			EvtSkillHitPos e;
			e.source = srcid;
			e.skill = skill;
			e.pos = target;
			_context.dispatcher().trigger(e);
			return false;
		});

		compShoot.tween.onStep([this, projectile](auto& t, float x, float y) {
			if (!_context.registry().valid(projectile)) {
				return false;
			}

			if (t.isFinished()) {
				_context.scene().destroyObject(projectile);
				return true;
			}

			auto& compTrans = _context.registry().get<CompTransform>(projectile);
			compTrans.position = { x, y };
			return false;
		});
	}

	void SkillSystem::onSkillHitPos(const EvtSkillHitPos& e)
	{
		if (!_context.registry().valid(e.source))
		{
			return;
		}

		auto& compSrcComm = _context.registry().get<CompComm>(e.source);
		auto& compAffect = _context.registry().get<CompSkillAffect>(e.skill);
		
		auto objects = _context.scene().getObjectsInCircle(e.pos, compAffect.range);
		for (auto& [d,obj] : objects) 
		{
			auto pCompDead = _context.registry().try_get<CompDead>(obj);
			if (pCompDead) 
			{
				continue;
			}

			auto pCompDstComm = _context.registry().try_get<CompComm>(obj);
			if (pCompDstComm && pCompDstComm->type == ObjectType::Npc && pCompDstComm->side != compSrcComm.side)
			{
				_context.dispatcher().trigger(EvtRoleOnAttack{ e.source, obj, e.skill });
			}
		}
	}

	void SkillSystem::onSkillHitTarget(const EvtSkillHitTarget& e)
	{
		if (!_context.registry().valid(e.source))
		{
			return;
		}

		auto pCompDead = _context.registry().try_get<CompDead>(e.target);
		if (pCompDead)
		{
			return;
		}

		auto& compSrcComm = _context.registry().get<CompComm>(e.source);
		auto& compDstComm = _context.registry().get<CompComm>(e.target);
		if (compDstComm.type == ObjectType::Npc && compDstComm.side != compSrcComm.side)
		{
			_context.dispatcher().trigger(EvtRoleOnAttack{ e.source, e.target, e.skill });
		}
	}

	void SkillSystem::onSkillEvent(const EvtExecSkillEvent& e)
	{
		LogInfo("onSkillEvent: event = source({}), skill({}), event({})", 
			(uint32_t)e.source, (uint32_t)e.skill, e.event);

		const auto& views = utility::StringUtil::split(e.event, ',');
		if (views.size() < 1) return;

		if (views[0] == "sky_turn_dark")
		{
			assert(views.size() == 2);

			int ticks = std::stoi(views[1].data());
			if (ticks < 2000) {
				LogError("sky_turn_dar time must greater than 2000");
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

		//LogInfo("object: id({}), cfg({}), name({}) On Attack !!! skill.cfg({}), skill.name({})", 
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
					//auto& dstTrans = _context.registry().get<CompTransform>(e.target);
					//dstTrans.position = { x, y };
					_context.scene().setObjectPos(e.target, { x, y });

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

	void SkillSystem::onTrapPeriodExec(entt::entity srcid, entt::entity skill, entt::entity trap)
	{
		if (!_context.registry().valid(skill) || !_context.registry().valid(trap))
		{
			LogError("src: {}, skill: {}, trap: {} NOT valid", srcid, skill, trap);
			return;
		}

		auto pCompTrans = _context.registry().try_get<CompTransform>(trap);
		if (!pCompTrans)
		{
			LogError("trap.transform or skill.traps NOT exist.");
			return;
		}

		EvtSkillHitPos e;
		e.source = srcid;
		e.skill = skill;
		e.pos = pCompTrans->position;
		_context.dispatcher().trigger(e);
	}
}