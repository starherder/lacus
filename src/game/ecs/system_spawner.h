#pragma once

#include "game/ecs/system_utils.h"

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

