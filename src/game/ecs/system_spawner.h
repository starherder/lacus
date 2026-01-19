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
        void onRoleDead(const RoleDead& e);

        void spawnActor(entt::entity spawner, const std::string& cfgid, const Vec2& pos, float radius);
    };
}

