#include "system_motion.h"
#include "comm_event.h"
#include "tweeny/tweeny.h"

namespace game 
{

    MotionSystem::MotionSystem(GameContext& context) : EcsSystem(context)
    {
        context.dispatcher().sink<MoveToGrid>().connect<&MotionSystem::onEventMoveToGrid>(this);
    }

    MotionSystem::~MotionSystem()
    {
    }

    void MotionSystem::update(float deltaTime)
    {
        deltaTime = std::clamp(deltaTime, 0.0f, 1.0f);

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
            spdlog::error("entity {} NOT found.", (uint32_t)id);
            return false;
        }

        auto& motion = _context.registry().get<CompMotion>(id);
        auto& transform = _context.registry().get<CompTransform>(id);

        const auto& srcPos = transform.position;
        const auto& dstPos = _context.currentScene().getGridCenterPos(dstGrid);

        Vec2i srcGrid = _context.currentScene().getGridFromPos(srcPos);
        if(srcGrid == dstGrid)
        {
            spdlog::warn("src_grid == dst_grid");
            return false;
        }

        if (findPath)
        {
            // path find
            auto path = _context.pathFinder().findPath(srcGrid, dstGrid);
            if (!path)
            {
                spdlog::info("path find failed.");
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
            spdlog::warn("path TOO short!,only {} grids", motion.path.size());
            return false;
        }

        motion.state = MotionState::Moving;
        motion.path_iterator = motion.path.rbegin();

        tweenNextGrid(id);

        return true;
    }

    bool MotionSystem::tweenNextGrid(entt::entity id)
    {
        auto& motion = _context.registry().get<CompMotion>(id);
        auto& transform = _context.registry().get<CompTransform>(id);

        motion.path_iterator++;
        if(motion.path_iterator == motion.path.rend())
        {
            transform.position = _context.currentScene().getGridCenterPos(motion.targetGrid);
            motionStop(id);
            return false;
        }

        const auto& curPos = transform.position;
        const auto& nextGrid = *motion.path_iterator;
        const auto& nextPos = _context.currentScene().getGridCenterPos(nextGrid);

        int ticks = (glm::distance(curPos, nextPos) / motion.speed) * 1000;
        motion.tween = tweeny::from(curPos.x, curPos.y)
            .to(nextPos.x, nextPos.y)
            .during(ticks)
            .via(motion.tween_mode.c_str())
            .onStep([&transform](auto& t, float x, float y) {
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

        _context.dispatcher().trigger(MotionStop{ id });
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

    void MotionSystem::onEventMoveToGrid(const MoveToGrid& e)
    {
        bool res = motionStart(e.actor, e.dest, e.findPath);
        if (!res) 
        {
            motionStop(e.actor);
        }
    }

}

