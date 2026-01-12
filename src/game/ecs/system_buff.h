#pragma once


#include "comm_system.h"
#include "comm_comp.h"
#include "comm_event.h"
#include "game/scene/game_camera.h"
#include "game/scene/game_scene.h"


namespace game
{

    class BuffSystem : public EcsSystem
    {
    public:
        BuffSystem(GameContext& context);
        ~BuffSystem();

        void update(float delta) override;

    private:
        void onAddBuffToObject(const AddBuffToObject& e);

        void onRemoveBuffFromObject(const RemoveBuffFromObject& e);

        void onPeriodExec(entt::entity target, entt::entity buff);
    };




}