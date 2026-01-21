#include "system_motion.h"
#include "comm_event.h"
#include "tweeny/tweeny.h"

namespace game 
{

    MotionSystem::MotionSystem(GameContext& context) : EcsSystem(context)
    {
        context.dispatcher().sink<EvtMoveToGrid>().connect<&MotionSystem::onEventMoveToGrid>(this);
    }

    MotionSystem::~MotionSystem()
    {
    }

    void MotionSystem::update(float deltaTime)
    {
        deltaTime = std::clamp(deltaTime, 0.0f, 1.0f);

        auto shoot_view = _context.registry().view<CompShoot>();
        for (auto& ent : shoot_view)
        {
            auto& compTween = shoot_view.get<CompShoot>(ent);
            auto delta = _context.frameTicker().deltaTicks();
            compTween.tween.step(delta);
        }

        auto ent_view = _context.registry().view<CompTransform, CompMotion>();
        for (auto& ent : ent_view)
        {
            auto& transform = ent_view.get<CompTransform>(ent);
            auto& motion = ent_view.get<CompMotion>(ent);

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

    bool MotionSystem::motionStart(entt::entity id, const Vec2i& dstGrid, bool findPath)
    {
        if (!_context.registry().valid(id))
        {
            SPDLOG_ERROR("entity {} NOT found.", (uint32_t)id);
            return false;
        }

        auto& motion = _context.registry().get<CompMotion>(id);
        auto& transform = _context.registry().get<CompTransform>(id);

        const auto& srcPos = transform.position;
        const auto& dstPos = _context.currentScene().getGridCenterPos(dstGrid);

        Vec2i srcGrid = _context.currentScene().getGridFromPos(srcPos);
        if(srcGrid == dstGrid)
        {
            //SPDLOG_WARN("src_grid == dst_grid");
            return false;
        }

        if (findPath)
        {
            // path find
            auto path = _context.pathFinder().findPath(srcGrid, dstGrid);
            if (!path)
            {
                SPDLOG_INFO("path find failed.");
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
            SPDLOG_WARN("path TOO short!,only {} grids", motion.path.size());
            return false;
        }

        motion.state = MotionState::Moving;
        motion.targetGrid = dstGrid;
        motion.path_iterator = motion.path.rbegin();

        tweenNextGrid(id);

        return true;
    }

    bool MotionSystem::tweenNextGrid(entt::entity entid)
    {
        auto& motion = _context.registry().get<CompMotion>(entid);
        auto& transform = _context.registry().get<CompTransform>(entid);

        motion.path_iterator++;
        if(motion.path_iterator == motion.path.rend())
        {
            transform.position = _context.currentScene().getGridCenterPos(motion.targetGrid);
            motionStop(entid);
            return false;
        }

        const auto& curPos = transform.position;
        const auto& nextGrid = *motion.path_iterator;
        const auto& curGrid = _context.currentScene().getGridFromPos(curPos);
        const auto& nextPos = _context.currentScene().getGridCenterPos(nextGrid);

        std::string mode = _context.gameConfig().motion.walk;

        auto curtype = _context.currentScene().getGridWalkType(curGrid);
        auto nexttype = _context.currentScene().getGridWalkType(nextGrid);
        if(curtype == (int)tilemap::WalkType::Swim && nexttype == (int)tilemap::WalkType::Swim)
        {
            mode = _context.gameConfig().motion.swim;
        }

        int ticks = (glm::distance(curPos, nextPos) / motion.speed) * 1000;
        motion.tween = tweeny::from(curPos.x, curPos.y)
            .to(nextPos.x, nextPos.y)
            .during(ticks)
            .via(mode.c_str())
            .onStep([&transform, entid, this](auto& t, float x, float y)
                {
                    checkEntityGrid(entid, transform.position, Vec2{x, y});
                    transform.position = { x, y };
                    return false;
                });

        return true;
    }

    bool MotionSystem::motionStop(entt::entity id)
    {
        if (_context.registry().valid(id))
        {
            auto& motion = _context.registry().get<CompMotion>(id);
            motion.state = MotionState::Resting;
            motion.path.clear();
            motion.path_iterator = motion.path.rbegin();
        }

        _context.dispatcher().trigger(EvtMotionStop{ id });
        return true;
    }

    bool MotionSystem::motionPause(entt::entity id, bool pause)
    {
        if (_context.registry().valid(id))
        {
            auto& motion = _context.registry().get<CompMotion>(id);
            motion.state = pause ? MotionState::Paused : MotionState::Moving;
        }

        return true;
    }

    void MotionSystem::onEventMoveToGrid(const EvtMoveToGrid& e)
    {
        bool res = motionStart(e.actor, e.dest, e.findPath);
        if (!res) 
        {
            motionStop(e.actor);
        }
    }

    void MotionSystem::checkEntityGrid(entt::entity ent, const Vec2& lstpos, const Vec2& curpos)
    {
        auto lstgrid = _context.currentScene().getGridFromPos(lstpos);
        auto curgrid = _context.currentScene().getGridFromPos(curpos);
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

