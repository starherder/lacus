#include "system_buff.h"



namespace game
{

	BuffSystem::BuffSystem(GameContext& context) : EcsSystem(context)
	{
		_context.dispatcher().sink<EvtAddBuff>().connect<&BuffSystem::onAddBuffToObject>(this);
		_context.dispatcher().sink<EvtRemoveBuff>().connect<&BuffSystem::onRemoveBuffFromObject>(this);
	}

	BuffSystem::~BuffSystem()
	{
	}

	void BuffSystem::update(float delta)
	{
		auto views = _context.registry().view<CompTransform, CompBuffComm, CompDisplay>();
		for(auto& ent : views)
		{
			auto& trans = views.get<CompTransform>(ent);
			auto& buff = views.get<CompBuffComm>(ent);
			auto& display = views.get<CompDisplay>(ent);

			if(!_context.registry().valid(buff.owner))
			{
				SPDLOG_INFO("buff({}) owner({}) NOT valid, remove buff({}).", 
							(uint32_t)buff.owner, (uint32_t)ent, buff.cfgid);
				_context.registry().emplace_or_replace<CompDestroy>(ent);
				continue;
			}
		
			// TODO: multi buff, arrange the pos
			auto& ownerTrans = _context.registry().get<CompTransform>(buff.owner);
			trans.position = ownerTrans.position + Vec2{ownerTrans.size.x/2, -ownerTrans.size.y/2} + trans.size/2.0f;

			buff.onUpdate(_context.applicaton().frameTicker().deltaTicks());
		}

		//-----------------------------------------------------------------------------

		auto buffsview = _context.registry().view<CompBuffs>();
		for(auto& ent : buffsview)
		{
			auto& buffs = buffsview.get<CompBuffs>(ent).buffs;

			for (auto it = buffs.begin(); it != buffs.end(); )
			{
				auto& buff = *it;
				if (!_context.registry().valid(buff))
				{
					it = buffs.erase(it);
					continue;
				}
				it++;
			}
		}
	}


	void BuffSystem::onAddBuffToObject(const EvtAddBuff& e)
	{
		auto buff = _context.objectFactory().createBuff(e.target, e.cfgid);
		if(!_context.registry().valid(buff))
		{
			//SPDLOG_ERROR("add buff ({}) failed.", e.cfgid);
			return;
		}

		auto pbuffs = _context.registry().try_get<CompBuffs>(e.target);
		if(!pbuffs)
		{
			//SPDLOG_ERROR("object ({}) can NOT hold buffs.", (uint32_t)e.target);
			return;
		}

		pbuffs->buffs.push_back(buff);

		auto& buffComm = _context.registry().get<CompBuffComm>(buff);
		buffComm.onUpdate = [this, e, buff](int64_t ticks) {
			if (!_context.registry().valid(buff))
			{
				return;
			}

			auto& buffComm = _context.registry().get<CompBuffComm>(buff);
			if(buffComm.duration > 0 )
			{
				buffComm.during_ticks += ticks;
				if(buffComm.during_ticks > buffComm.duration)
				{
					//SPDLOG_INFO("buff({}) expired, remove buf({})", (uint32_t)buff, buffComm.cfgid);
					_context.registry().emplace_or_replace<CompDestroy>(buff);
					return;
				}
			}

			buffComm.period_flag += ticks;
			if(buffComm.period_flag > buffComm.period)
			{
				onPeriodExec(e.target, buff);
				
				buffComm.period_flag = 0;
			}
		};
	}

	void BuffSystem::onRemoveBuffFromObject(const EvtRemoveBuff& e)
	{
		auto buff = _context.objectFactory().createBuff(e.target, e.cfgid);
		if (!_context.registry().valid(buff))
		{
			SPDLOG_ERROR("add buff ({}) failed.", e.cfgid);
			return;
		}

		auto pbuffs = _context.registry().try_get<CompBuffs>(e.target);
		if (!pbuffs)
		{
			SPDLOG_ERROR("object ({}) can NOT hold buffs.", (uint32_t)e.target);
			return;
		}

		auto& buffs = pbuffs->buffs;

		for(auto it=buffs.begin(); it!=buffs.end(); )
		{
			auto& buff = *it;
			if(_context.registry().valid(buff))
			{
				auto& bufComm = _context.registry().get<CompBuffComm>(buff);
				if(bufComm.cfgid == e.cfgid)
				{
					it = buffs.erase(it);
					continue;
				}
			}

			it++;
		}
	}


	void BuffSystem::onPeriodExec(entt::entity target, entt::entity buff)
	{
		// 改造 FightSystem::onRoleUnderAttack，发消息过去处理伤害

		auto& buffComm = _context.registry().get<CompBuffComm>(buff);
		//SPDLOG_INFO("buff({}) period exec : {}", buffComm.cfgid, buffComm.func);

		EvtAddSkillFuncs func;
		func.source = entt::null;
		func.target = target;
		func.funcs = buffComm.func;
		_context.dispatcher().trigger(func);
	}




}