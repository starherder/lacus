#include "system_fight.h"



namespace game 
{
	FightSystem::FightSystem(GameContext& context) : EcsSystem(context) 
	{
		_context.dispatcher().sink<RoleExecSkillToObject>().connect<&FightSystem::onRoleExecSkillToObject>(this);
	}
	
	FightSystem::~FightSystem() 
	{
	}

	void FightSystem::update(float delta)
	{
		auto deltaTicks = _context.frameTicker().deltaTicks();

		auto views = _context.registry().view<CompNameId, CompSkillComm>();
		for (auto& ent : views)
		{
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

		auto& srcTrans = _context.registry().get<CompTransform>(e.source);
		auto& dstTrans = _context.registry().get<CompTransform>(e.target);

		auto& srcPos = srcTrans.position;
		auto& dstPos = dstTrans.position;

		auto& skillAffect = _context.registry().get<CompSkillAffect>(e.skill);
		auto& skillTween = _context.registry().get<CompSkillTween>(e.skill);

		skillTween.tween = tweeny::from(srcPos.x, srcPos.y)
			.to(dstPos.x, dstPos.y)
			.via(skillTween.prev_tween)
			.during(skillAffect.prev_ticks)
			.onStep([e, this](auto& t, float x, float y) 
			{
				if (_context.registry().valid(e.source) == false)
				{
					return false;
				}

				if (t.isFinished()) 
				{	
					skillAffectApplyToObject(e);

					return false;
				}

				auto& srcTrans = _context.registry().get<CompTransform>(e.source);
				srcTrans.position = {x, y};

				return false;
			});
	}


	void FightSystem::skillAffectApplyToObject(const RoleExecSkillToObject& e)
	{
		spdlog::info("skill affect !");

		auto& skillComm = _context.registry().get<CompSkillComm>(e.skill);
		skillComm.state = SkillState::Cooling;

		auto& skillAffect = _context.registry().get<CompSkillAffect>(e.skill);
		_context.dispatcher().trigger(ExecSkillEvent{e.source, e.skill, skillAffect.event});
	}






}