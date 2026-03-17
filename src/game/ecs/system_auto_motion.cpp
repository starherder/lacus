#include "system_auto_motion.h"
#include "comm_event.h"
#include "tweeny/tweeny.h"
#include "game/game_config.h"


namespace game 
{
    DeclareEcsSystem(AutoMotionSystem, EcsPriority::Middle);


    AutoMotionSystem::AutoMotionSystem(GameContext& context) : EcsSystem(context)
    {
        context.dispatcher().sink<EvtMoveToGrid>().connect<&AutoMotionSystem::onEventMoveToGrid>(this);
        context.dispatcher().sink<EvtMotionSwitchState>().connect<&AutoMotionSystem::onEventMotionStateSwtich>(this);
        context.dispatcher().sink<EvtRoleStopMotion>().connect<&AutoMotionSystem::onEventStopMotion>(this);
    }

    AutoMotionSystem::~AutoMotionSystem()
    {
    }

    void AutoMotionSystem::update(float deltaTime)
    {
        deltaTime = std::clamp(deltaTime, 0.0f, 1.0f);

        auto shoot_view = _context.registry().view<CompShoot>();
        for (auto& ent : shoot_view)
        {
            auto& compTween = shoot_view.get<CompShoot>(ent);
            auto delta = _context.frameTicker().deltaTicks();
            compTween.tween.step(delta);
        }

        auto ent_view = _context.registry().view<CompAutoMotion>();
        for (auto& ent : ent_view)
        {
            auto pdead = _context.registry().try_get<CompDead>(ent);
            if (pdead) 
            { 
                continue; 
            }

            auto& motion = ent_view.get<CompAutoMotion>(ent);
            if (motion.state != MotionState::Moving)
            {
                continue;
            }

            auto delta = _context.frameTicker().deltaTicks();
            motion.tween.step(delta);

            if(motion.tween.progress() >= 1.0f)
            {
                tweenNextGrid(ent);
            }
        }
    }

    bool AutoMotionSystem::motionStart(entt::entity id, const Vec2i& dstGrid, bool findPath)
    {
        if (!_context.registry().valid(id))
        {
            LogError("entity {} NOT found.", (uint32_t)id);
            return false;
        }

        auto pmotion = _context.registry().try_get<CompAutoMotion>(id);
        if(!pmotion)
        {
            _context.registry().emplace_or_replace<CompAutoMotion>(id, CompAutoMotion{});
        }

        auto& move = _context.registry().get<CompMoveCfg>(id);
        auto& transform = _context.registry().get<CompTransform>(id);
        auto& motion = _context.registry().get<CompAutoMotion>(id);

        const auto& srcPos = transform.position;
        const auto& dstPos = _context.scene().getGridCenterPos(dstGrid);

        Vec2i srcGrid = _context.scene().getGridFromPos(srcPos);
        if(srcGrid == dstGrid)
        {
            //LogWarn("src_grid == dst_grid");
            return false;
        }

        if (findPath)
        {
            // path find
            auto path = _context.findPath(srcGrid, dstGrid, move.swim_speed > 0.0f);
            if (!path)
            {
                LogInfo("path find failed.");
                motionStop(id);
                return false;
            }
            else
            {
                motion.path.swap(path.value());
            }
        }

        if(motion.path.size() < 2) 
        {
            LogWarn("path TOO short!,only {} grids", motion.path.size());
            return false;
        }

        motion.state = MotionState::Moving;
        motion.targetGrid = dstGrid;
        motion.path_iterator = motion.path.rbegin();

        tweenNextGrid(id);

        return true;
    }

    bool AutoMotionSystem::tweenNextGrid(entt::entity entid)
    {
        auto& move = _context.registry().get<CompMoveCfg>(entid);
        auto& motion = _context.registry().get<CompAutoMotion>(entid);
        auto& transform = _context.registry().get<CompTransform>(entid);

        motion.path_iterator++;
        if(motion.path_iterator == motion.path.rend())
        {
            //transform.position = _context.scene().getGridCenterPos(motion.targetGrid);
            auto pos = _context.scene().getGridCenterPos(motion.targetGrid);
            _context.scene().setObjectPos(entid, pos);

            motionStop(entid);
            return false;
        }

        const auto& curPos = transform.position;
        const auto& nextGrid = *motion.path_iterator;
        const auto& curGrid = _context.scene().getGridFromPos(curPos);
        const auto& nextPos = _context.scene().getGridCenterPos(nextGrid);

        auto curType = _context.scene().getGridWalkType(curGrid);
        auto nextType = _context.scene().getGridWalkType(nextGrid);

        float moveSpeed = move.speed;
        std::string tweenMode = _context.gameConfig().motion.walk;

        if(curType == (int)tilemap::WalkType::Swim 
            && nextType == (int)tilemap::WalkType::Swim)
        {
            moveSpeed = move.swim_speed;
            tweenMode = _context.gameConfig().motion.swim;
        }

        int ticks = (glm::distance(curPos, nextPos) / moveSpeed) * 1000;
        motion.tween = tweeny::from(curPos.x, curPos.y)
            .to(nextPos.x, nextPos.y)
            .during(ticks)
            .via(tweenMode.c_str())
            .onStep([entid, this](auto& t, float x, float y)
                {
                    checkEntityGrid(entid, Vec2{x, y});

                    _context.scene().setObjectPos(entid, {x, y});
                    return false;
                });

        return true;
    }

    bool AutoMotionSystem::motionStop(entt::entity id)
    {
        if (_context.registry().valid(id))
        {
            auto motion = _context.registry().try_get<CompAutoMotion>(id);
            if (motion)
            {
                motion->state = MotionState::Resting;
                motion->path.clear();
                motion->path_iterator = motion->path.rbegin();
            }
        }

        _context.dispatcher().trigger(EvtMotionStop{ id });
        return true;
    }

    bool AutoMotionSystem::motionPause(entt::entity id, bool pause)
    {
        if (_context.registry().valid(id))
        {
            auto& motion = _context.registry().get<CompAutoMotion>(id);
            motion.state = pause ? MotionState::Paused : MotionState::Moving;
        }

        return true;
    }

    void AutoMotionSystem::onEventMoveToGrid(const EvtMoveToGrid& e)
    {
        if(_context.gamePlay().getType() == GamePlay_TileBattle)
        {
            return;
        }

        bool res = motionStart(e.actor, e.dest, e.findPath);
        if (!res) 
        {
            motionStop(e.actor);
        }
    }
    
    void AutoMotionSystem::onEventStopMotion(const EvtRoleStopMotion& e)
    {
        motionStop(e.actor);
    }

    void AutoMotionSystem::onEventMotionStateSwtich(const EvtMotionSwitchState& e)
    {
        if (_context.registry().valid(e.actor))
        {
            auto pmotion = _context.registry().try_get<CompAutoMotion>(e.actor);
            if(pmotion) 
            {
                pmotion->state = e.new_state;
            }
        }
    }

    void AutoMotionSystem::checkEntityGrid(entt::entity ent, const Vec2& curpos)
    {
        auto& transform = _context.registry().get<CompTransform>(ent);

        auto lstgrid = _context.scene().getGridFromPos(transform.position);
        auto curgrid = _context.scene().getGridFromPos(curpos);
        if(curgrid != lstgrid) 
        {
            EvtRoleCrossGrid e;
            e.actor = ent;
            e.cur_grid = curgrid;
            e.lst_grid = lstgrid;
            _context.dispatcher().trigger(e);
        }
    }

}

