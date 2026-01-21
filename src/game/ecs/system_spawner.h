#pragma once


#include "comm_system.h"
#include "comm_comp.h"
#include "comm_event.h"
#include "game/scene/game_camera.h"
#include "game/scene/game_scene.h"


namespace game {

    class SpawnerSystem : public EcsSystem
    {
    public:
        SpawnerSystem(GameContext& context);
        ~SpawnerSystem();

        void update(float deltaTime);

    private:
        void onRoleDead(const EvtRoleDead& e);

        void spawnActor(entt::entity spawner, const CompSpawner& compSpawner, const CompTransform& trans);
    };
}

