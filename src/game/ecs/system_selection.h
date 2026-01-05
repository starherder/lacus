#pragma once


#include "comm_system.h"
#include "comm_comp.h"
#include "comm_event.h"
#include "game/scene/game_camera.h"
#include "game/scene/game_scene.h"


namespace game 
{

    class SelectionSystem : public EcsSystem
    {
    public:
        SelectionSystem(GameContext& context);
        ~SelectionSystem();

        void update(float delta) override;

    private:
        void onObjectSelection(const ObjectSelection& e);
    };




}