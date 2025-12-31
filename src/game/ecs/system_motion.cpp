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
        auto ent_view = _context.registry().view<CompTransform, CompMotion, CompState>();
        for (auto& ent : ent_view)
        {
            auto& transform = ent_view.get<CompTransform>(ent);
            auto& motion = ent_view.get<CompMotion>(ent);
            auto& state = ent_view.get<CompState>(ent);
            auto& pos = transform.position;

            if (state.state == ActorState::Move)
            {
                if (!motion.running)
                {
                    continue;
                }

                if (motion.path.empty())
                {
                    if (glm::distance(motion.targetPos, pos) <= motion.speed * deltaTime)
                    {
                        spdlog::info("{} motion finish: at ({}, {})", (int32_t)ent, motion.targetPos.x, motion.targetPos.y);

                        motionStop(ent);
                        setActorPos(ent, motion.targetPos);
                    }
                    continue;
                }

                const auto& cur_grid = _context.currentScene().getGridFromPos(pos);
                const auto& next_grid = motion.path.back();

                if (next_grid != cur_grid)
                {
                    motion.velocity = glm::normalize(_context.currentScene().getGridCenterPos(next_grid) - pos) * motion.speed;
                }
                else
                {
                    if (reachGridCenter(pos, next_grid, motion.speed * deltaTime))
                    {
                        spdlog::info("{} motion: reach ({}, {}), go next step ({}, {})",
                            (int32_t)ent, cur_grid.x, cur_grid.y, next_grid.x, next_grid.y);

                        motion.path.pop_back();
                    }
                }

                transform.position += motion.velocity * deltaTime;
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

        float dis = glm::distance(pos, grid_pos);
        spdlog::info("dis: {}, epsilon: {}", dis, epsilon);

        return dis < epsilon;
    }

    bool MotionSystem::motionStart(entt::entity id, const Vec2& dst)
    {
        if (!motionStop(id))
        {
            return false;
        }

        auto& transform = _context.registry().get<CompTransform>(id);
        auto& motion = _context.registry().get<CompMotion>(id);
        auto& src = transform.position;

        // from world pos to grid
        Vec2i srcGrid = _context.currentScene().getGridFromPos(src);
        Vec2i dstGrid = _context.currentScene().getGridFromPos(dst);

        spdlog::info("{} motion start: ({}, {}) -> ({}, {})", (int32_t)id, srcGrid.x, srcGrid.y, dstGrid.x, dstGrid.y);

        // path find
        auto path = _context.pathFinder().findPath(srcGrid, dstGrid);
        if (path.empty())
        {
            spdlog::info("path find failed.");
            return false;
        }

        // add path
        motion.path.clear();
        for (auto& grid : path)
        {
            motion.path.push_back(grid);
        }

        // set state
        auto& state = _context.registry().get<CompState>(id);
        state.state = ActorState::Move;

        motion.velocity = glm::normalize(_context.currentScene().getGridCenterPos(motion.path.back()) - src) * motion.speed;
        motion.running = true;
        return true;
    }

    bool MotionSystem::motionStop(entt::entity id)
    {
        if (!_context.registry().valid(id))
        {
            spdlog::warn("entity {} not exist.", (int32_t)id);
            return false;
        }

        spdlog::info("motion stop: {} ");

        auto& motion = _context.registry().get<CompMotion>(id);
        motion.path.clear();
        motion.velocity = { 0, 0 };
        motion.running = false;

        auto& state = _context.registry().get<CompState>(id);
        state.state = ActorState::Idle;

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
        motion.running = !pause;
        return true;
    }


    void MotionSystem::onEventMoveToPos(const MoveToPos& e)
    {
        motionStart(e.actor, e.dest);
    }


}

