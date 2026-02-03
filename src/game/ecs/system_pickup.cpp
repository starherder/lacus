#include "system_pickup.h"
#include "game/game_config.h"

namespace game 
{

    DeclareEcsSystem(PickupSystem, EcsPriority::Middle);

    PickupSystem::PickupSystem(GameContext& context) : EcsSystem(context)
    {
        context.dispatcher().sink<EvtRoleCrossGrid>().connect<&PickupSystem::onEventMoveToGrid>(this);
    }

    PickupSystem::~PickupSystem()
    {
    }

    void PickupSystem::update(float deltaTime)
    {
        auto views = _context.registry().view<CompPickable>();
        for(auto& ent : views) 
        {
            auto& pickComp = _context.registry().get<CompPickable>(ent);
            if(pickComp.picked)
            {
                auto delta = _context.frameTicker().deltaTicks();
                pickComp.tween.step(delta);
            }
        }
    }

    void PickupSystem::pickUp(entt::entity role, entt::entity obj)
    {
        auto& nameComp = _context.registry().get<CompNameId>(obj);
        auto& transComp = _context.registry().get<CompTransform>(obj);

        // ´ÓËÄ²æÊ÷ÒÆ³ý
        _context.scene().removeObjectFromQuadtree(obj);

        // ÇÐ»»µ½ÆÁÄ»¿Õ¼ä
        _context.scene().swichCoord(transComp, CoordMode::ScreenSpace);

        const auto& curpos = transComp.position;
        const Vec2 uipos = { 0, 0 };

        auto& pickableComp = _context.registry().get<CompPickable>(obj);
        pickableComp.picked = true;

        _context.objectFactory().createParticleOnObject(obj, pickableComp.effect);

        pickableComp.tween = tweeny::from(curpos.x, curpos.y)
                                    .to(uipos.x, uipos.y)
                                    .via("quadraticIn")
                                    .during(pickableComp.use_ticks)
                                    .onStep([this, role, obj](auto& t, float x, float y) {
                                        if (!_context.registry().valid(obj)) 
                                        {
                                            return false;
                                        }

                                        auto& nameComp = _context.registry().get<CompNameId>(obj);
                                        auto& transComp = _context.registry().get<CompTransform>(obj);
                                        transComp.position = {x,y };

                                        if (t.isFinished()) 
                                        {
                                            _context.scene().destroyObject(obj);
                                            _context.dispatcher().trigger<EvtRolePickItemFinish>(EvtRolePickItemFinish{ role, obj });
                                        }

                                        return false;
                                    });
        
        _context.dispatcher().trigger<EvtRolePickItemStart>(EvtRolePickItemStart{role, obj});
    }

    void PickupSystem::useItem(entt::entity actor, entt::entity obj)
    {
        auto pActorComm = _context.registry().try_get<CompComm>(actor);
        auto pObjComm = _context.registry().try_get<CompComm>(obj);
        if(!pActorComm || pActorComm->type !=ObjectType::Npc || !pObjComm || pObjComm->type != ObjectType::Item)
        {
            return;
        }

        auto& pickableComp = _context.registry().get<CompPickable>(obj);
        pickableComp.picked = true;

        _context.objectFactory().createParticleOnObject(obj, pickableComp.effect);

        pickableComp.tween = tweeny::from(0.0f, 0.0f)
                                    .to(100.0f, 100.0f)
                                    .via("linear")
                                    .during(pickableComp.use_ticks)
                                    .onStep([this, obj](auto& t, float x, float y) {
                                        if (t.isFinished())
                                        {
                                            _context.scene().destroyObject(obj);
                                            return true;
                                        }
                                        auto& displayComp = _context.registry().get<CompDisplay>(obj);
                                        displayComp.ground_color.a = (int)x;
                                        return false;
                                    });

        EvtExecPropFuncs func;
        func.source = entt::null;
        func.target = actor;
        func.funcs = pickableComp.funcs;
        _context.dispatcher().trigger(func);
    }

    void PickupSystem::onEventMoveToGrid(const EvtRoleCrossGrid& e)
    {
        auto compComm = _context.registry().try_get<CompComm>(e.actor);
        auto compTrans = _context.registry().try_get<CompTransform>(e.actor);
        if (!compComm || !compTrans)
        {
            return;
        }

        if (compComm->type != ObjectType::Npc || compComm->side != CampSide::Gangster)
        {
            return;
        }

        auto center = compTrans->position;
        auto radius = glm::length(compTrans->size) + _context.gameConfig().pick_range;

        const auto& objects = _context.scene().getObjectsInCircle(center, radius);
        for(auto& [dis, obj] : objects) 
        {
            auto pickComp = _context.registry().try_get<CompPickable>(obj);
            if (pickComp && pickComp->picked == false)
            {
                if (pickComp->pick_use)
                {
                    useItem(e.actor, obj);
                }
                else
                {
                    pickUp(e.actor, obj);
                }
            }
        }
    }

}