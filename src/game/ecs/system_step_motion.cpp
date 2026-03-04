#include "system_step_motion.h"
#include "game/game_config.h"
#include "game/logic/game_play.h"

namespace game
{
    DeclareEcsSystem(StepMotionSystem, EcsPriority::Middle);

    StepMotionSystem::StepMotionSystem(GameContext& context) : EcsSystem(context)
    {
        context.dispatcher().sink<EvtStepMove>().connect<&StepMotionSystem::onEventStepMove>(this);
    }
    
    StepMotionSystem::~StepMotionSystem()
    {
    }

    void StepMotionSystem::update(float delta)
    {
        auto ent_view = _context.registry().view<CompStepMotion>();
        for (auto& ent : ent_view)
        {
            auto pdead = _context.registry().try_get<CompDead>(ent);
            if (pdead)
            {
                continue;
            }

            auto& motion = ent_view.get<CompStepMotion>(ent);
            if (motion.state != MotionState::Moving)
            {
                continue;
            }

            auto delta = _context.frameTicker().deltaTicks();
            motion.tween.step(delta);
        }
    }

    void StepMotionSystem::onEventStepMove(const EvtStepMove& e)
    {
        auto pMotion = _context.registry().try_get<CompStepMotion>(e.actor);
        if (!pMotion)
        {
            _context.registry().emplace<CompStepMotion>(e.actor, CompStepMotion{});
        }

        auto& motion = _context.registry().get<CompStepMotion>(e.actor);
        if (motion.state != MotionState::Resting)
        {
            return;
        }

        motion.state = MotionState::Moving;
        tweenNextGrid(e.actor, e.dir);
    }

    Vec2i StepMotionSystem::getNextGrid(const Vec2i& curGrid, const Vec2i& dir)
    {
        return curGrid + dir;
    }

    bool StepMotionSystem::tweenNextGrid(entt::entity entid, const Vec2i& dir)
    {
        auto& move = _context.registry().get<CompMoveCfg>(entid);
        auto& motion = _context.registry().get<CompStepMotion>(entid);
        auto& transform = _context.registry().get<CompTransform>(entid);

        const auto& curPos = transform.position;
        const auto& curGrid = _context.scene().getGridFromPos(curPos);
        const auto& nextGrid = getNextGrid(curGrid, dir);
        const auto& nextPos = _context.scene().getGridCenterPos(nextGrid);

        auto curType = _context.scene().getGridWalkType(curGrid);
        auto nextType = _context.scene().getGridWalkType(nextGrid);

        float moveSpeed = move.speed;
        std::string tweenMode = _context.gameConfig().motion.walk;

        if (curType == (int)tilemap::WalkType::Swim
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
            if (t.isFinished()) 
            {
                onMotionStop(entid);
                return true;
            }

            _context.scene().setObjectPos(entid, { x, y });
            return false;
        });

        return true;
    }

    void StepMotionSystem::onMotionStop(entt::entity entid)
    {
        if (!_context.registry().valid(entid))
        {
            return;
        }
        
        auto pmotion = _context.registry().try_get<CompStepMotion>(entid);
        if (pmotion)
        {
            pmotion->state = MotionState::Resting;
        }

        _context.gamePlay().onMotionFinish(entid);
    }

}