#include "system_motion.h"
#include "comm_event.h"

namespace game 
{
    MotionSystem::MotionSystem(GameContext& context) : EcsSystem(context)
    {
        context.dispatcher().sink<MoveToPos>().connect<&MotionSystem::onEventMoveToPos>(this);
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
            const auto& pos = transform.position;

            if (motion.state == MotionState::Moving)
            {
                if (motion.path.empty())
                {
                    motion.velocity = glm::normalize(motion.targetPos - pos);

                    if (glm::distance(motion.targetPos, pos) <= motion.speed * deltaTime)
                    {
                        transform.position = motion.targetPos;
                        motionStop(ent);
                    }
                }
                else
                {
                    const auto& cur_grid = _context.currentScene().getGridFromPos(pos);
                    const auto& next_grid = motion.path.back();
                    if (next_grid != cur_grid)
                    {
                        motion.velocity = glm::normalize(_context.currentScene().getGridCenterPos(next_grid) - pos);
                    }
                    else
                    {
                        if (reachGridCenter(pos, next_grid, motion.speed * deltaTime))
                        {
                            motion.path.pop_back();
                        }
                    }
                }
                transform.position += motion.velocity * motion.speed * deltaTime;
            }
        }
	}

    void MotionSystem::setActorPos(entt::entity id, const Vec2& pos)
    {
        if (!_context.registry().valid(id))
        {
            spdlog::warn("entity {} not exist.", (int32_t)id);
            return;
        }

        auto& transform = _context.registry().get<CompTransform>(id);
        transform.position = pos;
    }

    Vec2 MotionSystem::getActorPos(entt::entity id)
    {
        if (!_context.registry().valid(id))
        {
            spdlog::warn("entity {} not exist.", (int32_t)id);
            return {};
        }
        return _context.registry().get<CompTransform>(id).position;
    }

    void MotionSystem::setMotionSpeed(entt::entity id, float speed)
    {
        if (!_context.registry().valid(id))
        {
            spdlog::warn("entity {} not exist.", (int32_t)id);
            return;
        }
        auto& motion = _context.registry().get<CompMotion>(id);
        motion.speed = speed;
    }

    float MotionSystem::getMotionSpeed(entt::entity id)
    {
        if (!_context.registry().valid(id))
        {
            spdlog::warn("entity {} not exist.", (int32_t)id);
            return 0.0f;
        }
        return _context.registry().get<CompMotion>(id).speed;
    }

    bool MotionSystem::reachGridCenter(const Vec2& pos, const Vec2i& grid, float epsilon)
    {
        auto grid_pos = _context.currentScene().getGridCenterPos(grid);
        return glm::distance(pos, grid_pos) < epsilon;
    }

    bool MotionSystem::motionStart(entt::entity id, const Vec2& dst, bool findPath)
    {
        auto& transform = _context.registry().get<CompTransform>(id);
        auto& motion = _context.registry().get<CompMotion>(id);
        const auto& src = transform.position;

        // from world pos to grid
        Vec2i srcGrid = _context.currentScene().getGridFromPos(src);
        Vec2i dstGrid = _context.currentScene().getGridFromPos(dst);

        if (findPath)
        {
            // path find
            auto path = _context.pathFinder().findPath(srcGrid, dstGrid);
            if (path)
            {
                spdlog::info("path find failed.");
                motionStop(id);
                return false;
            }

            // add path
            motion.path.clear();
            for (auto& grid : path.value())
            {
                motion.path.push_back(grid);
            }
        }

        motion.targetPos = dst;
        motion.state = MotionState::Moving;
        motion.velocity = glm::normalize(_context.currentScene().getGridCenterPos(motion.path.back()) - src);
        return true;
    }

    bool MotionSystem::motionStop(entt::entity id)
    {
        if (!_context.registry().valid(id))
        {
            spdlog::warn("entity {} not exist.", (int32_t)id);
            return false;
        }

        auto& motion = _context.registry().get<CompMotion>(id);
        motion.path.clear();
        motion.velocity = { 0, 0 };
        motion.state = MotionState::Resting;

        _context.dispatcher().trigger(MotionStop{id});
        return true;
    }

    bool MotionSystem::motionPause(entt::entity id, bool pause)
    {
        if (!_context.registry().valid(id))
        {
            spdlog::warn("entity {} not exist.", (int32_t)id);
            return false;
        }

        auto& motion = _context.registry().get<CompMotion>(id);
        motion.state = pause ? MotionState::Paused : MotionState::Moving;
        return true;
    }

    void MotionSystem::onEventMoveToPos(const MoveToPos& e)
    {
        motionStart(e.actor, e.dest, e.findPath);
    }


}

