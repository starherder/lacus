#pragma once


#include "game/ecs/system_utils.h"

namespace game
{

    class StepMotionSystem : public EcsSystem
    {
    public:
        StepMotionSystem(GameContext& context);
        ~StepMotionSystem();

        void update(float delta) override;
        
    private:
        void onEventStepMove(const EvtStepMove& e);

        bool tweenNextGrid(entt::entity entid, const Vec2i& dir);

        Vec2i getNextGrid(const Vec2i& curGrid, const Vec2i& dir);

        void onMotionStop(entt::entity entid);

        void onEventGameTurnStart(const EvtGameTurnFinish& e);
        void onEventGameTurnFinish(const EvtGameTurnFinish& e);
    };



}