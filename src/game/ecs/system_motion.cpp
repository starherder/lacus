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

        auto ent_view = _context.registry().view<CompTransform, CompMotion, CompState>();
        for (auto& ent : ent_view)
        {
            auto& transform = ent_view.get<CompTransform>(ent);
            auto& motion = ent_view.get<CompMotion>(ent);
            auto& state = ent_view.get<CompState>(ent);
            const auto& pos = transform.position;

            if (state.state == ActorState::Move)
            {
                if (!motion.running)
                {
                    continue;
                }

                if (motion.path.empty())
                {
                    motion.velocity = glm::normalize(motion.targetPos - pos);

                    if (glm::distance(motion.targetPos, pos) <= motion.speed * deltaTime)
                    {
                        spdlog::info("{} motion finish: at ({}, {})", (int32_t)ent, motion.targetPos.x, motion.targetPos.y);

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
                            spdlog::info("{} motion: reach ({}, {}), go next step ({}, {})",
                                (int32_t)ent, cur_grid.x, cur_grid.y, next_grid.x, next_grid.y);

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

        float dis = glm::distance(pos, grid_pos);
        //spdlog::info("dis: {}, epsilon: {}", dis, epsilon);

        return dis < epsilon;
    }

    bool MotionSystem::motionStart(entt::entity id, const Vec2& dst, bool findPath)
    {
        auto& transform = _context.registry().get<CompTransform>(id);
        auto& motion = _context.registry().get<CompMotion>(id);
        const auto& src = transform.position;

        // from world pos to grid
        Vec2i srcGrid = _context.currentScene().getGridFromPos(src);
        Vec2i dstGrid = _context.currentScene().getGridFromPos(dst);

        spdlog::info("{} motion start: ({}, {})[{},{}] -> ({}, {})[{},{}]",
            (int32_t)id, src.x, src.y, srcGrid.x, srcGrid.y, dst.x, dst.y, dstGrid.x, dstGrid.y);

        if (findPath)
        {
            // path find
            auto path = _context.pathFinder().findPath(srcGrid, dstGrid);
            if (path.empty())
            {
                spdlog::info("path find failed.");
                motionStop(id);
                return false;
            }

            std::string pathlist;

            // add path
            motion.path.clear();
            for (auto& grid : path)
            {
                pathlist += fmt::format("[{},{}]", grid.x, grid.y);

                motion.path.push_back(grid);
            }

            spdlog::info("pathlist = {}", pathlist);
        }

        // set state
        auto& state = _context.registry().get<CompState>(id);
        state.state = ActorState::Move;

        motion.targetPos = dst;
        motion.running = true;
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
        spdlog::info("MotionSystem::onEventMoveToPos  e.actor = {}, e.dest = ({},{})", (int)e.actor, e.dest.x, e.dest.y);

        motionStart(e.actor, e.dest, e.findPath);
    }


}

