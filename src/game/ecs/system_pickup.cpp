#include "system_pickup.h"


namespace game 
{

    PickupSystem::PickupSystem(GameContext& context) : EcsSystem(context)
    {
        context.dispatcher().sink<RoleCrossGrid>().connect<&PickupSystem::onEventMoveToGrid>(this);
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

    void PickupSystem::pickUp(entt::entity role, entt::entity obj, const Vec2i& grid)
    {
        Vec2f curpos = _context.currentScene().getGridCenterPos(grid);
        Vec2f uipos = _context.camera().screenToWorld({0,0});

        auto& nameComp = _context.registry().get<CompNameId>(obj);
        spdlog::info("pickUp: obj.id = {}, obj.name = {}, obj.cfg = {}", (int)nameComp.id, nameComp.name, nameComp.cfg_id);

        auto& transComp = _context.registry().get<CompTransform>(obj);
        auto& pickableComp = _context.registry().get<CompPickable>(obj);

        pickableComp.picked = true;
        pickableComp.tween = tweeny::from(curpos.x, curpos.y)
                                    .to(uipos.x, uipos.y)
                                    .via("linear")
                                    .during(2000)
                                    .onStep([this, &transComp, &nameComp](auto& t, float x, float y) {
                                        transComp.position = {x,y };

                                        if (t.isFinished()) {
                                            spdlog::info("pickable object {} tween finish", nameComp.cfg_id);
                                            
                                            auto& commComp = _context.registry().get<CompComm>(nameComp.id);
                                            commComp.state = LifeState::Destroy;
                                        }

                                        return false;
                                    });
        
        auto& display = _context.registry().get<CompDisplay>(obj);
        if(display.particle)
        {
            display.particle->Start();
        }
    }

    void PickupSystem::onEventMoveToGrid(const RoleCrossGrid& e)
    {
        auto& objects = _context.currentScene().getObjectsInGrid(e.cur_grid);
        for(auto& obj : objects) 
        {
            auto pickComp = _context.registry().try_get<CompPickable>(obj);
            if (pickComp && pickComp->picked == false)
            {
                pickUp(e.actor, obj, e.cur_grid);
            }
        }
    }
}