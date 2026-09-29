#include "system_trigger.h"

namespace game
{
    DeclareEcsSystem(TriggerSystem, EcsPriority::Middle);

    TriggerSystem::TriggerSystem(GameContext& context) : EcsSystem(context)
    {
        _context.dispatcher().sink<EvtRoleEnterTriggerArea>().connect<&TriggerSystem::onRoleEnterTriggerArea>(this);
    }

    TriggerSystem::~TriggerSystem()
    {
    }

    void TriggerSystem::update(float delta)
    {
    }

    void TriggerSystem::onRoleEnterTriggerArea(const EvtRoleEnterTriggerArea& e)
    {
        if (!_context.registry().valid(e.actor) || !_context.registry().valid(e.trigger))
        {
            return;
        }

        auto trigger = _context.registry().try_get<CompTriggerArea>(e.trigger);
        if (!trigger || trigger->event.empty() || (trigger->once && trigger->triggered))
        {
            return;
        }

        trigger->triggered = true;
        //_context.dispatcher().trigger(EvtExecTriggerEvent{ e.actor, e.trigger, trigger->event });

        auto pSrcComm = _context.registry().try_get<CompNameId>(e.actor);
        auto pDstComm = _context.registry().try_get<CompNameId>(e.trigger);
        if(!pSrcComm || !pDstComm)
        {
			LogError("srcid ({}) or dst({}) not exist.", (uint64_t)e.actor, (uint64_t)e.trigger);
            return;
        }

        LogInfo("src ({}) trigger ({}) event ({})", pSrcComm->name, pDstComm->name, trigger->event);
    }
}
