#pragma once


#include "game/ecs/system_utils.h"

namespace game 
{

    class SelectionSystem : public EcsSystem
    {
    public:
        SelectionSystem(GameContext& context);
        ~SelectionSystem();

        void update(float delta) override;

    private:
        void onObjectSelection(const EvtObjectSelection& e);
    };



}