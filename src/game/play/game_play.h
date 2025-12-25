#pragma once

#include "engine/wrapper.h"
#include "components.h"
#include <unordered_map>

namespace engine {
    class Renderer;
};

namespace AStar {
    class Generator;
}

namespace game {

    using namespace engine;

class GamePlay
{
public:
    GamePlay();
    ~GamePlay();

    void initPathFind(AStar::Generator* generator, const Vec2i& mapSize, const Vec2i& tileSize);
    void setDebugMode(bool enable) { _debugMode = enable; }

    void update(float deltaTime);
    void draw(Renderer& renderer);

    entt::entity createActor(const std::string& name, const Vec2& pos, const Vec2& size={64, 64});
    entt::entity getActor(const std::string& name);

    void setActorPos(entt::entity id, const Vec2& pos);
    Vec2 getActorPos(entt::entity id);
    
    void setMotionSpeed(entt::entity id, float speed);
    float getMotionSpeed(entt::entity id);

    bool motionStart(entt::entity id, const Vec2& dst);
    bool motionPause(entt::entity id, bool pause);
    bool motionStop(entt::entity id);

    ActorState getActorState(entt::entity id);

    void destroyActor(entt::entity id);
    void destroyActor(const std::string& name);
    void destroyAllActor();

private:
    void motionSystem(float deltaTime);
    void renderSystem(Renderer& renderer);

    Vec2i getGridFromPos(const Vec2& pos);
    Vec2 getGridLeftTopPos(const Vec2i& grid);
    Vec2 getGridCenterPos(const Vec2i& grid);

    bool reachGridCenter(const Vec2& pos, const Vec2i& grid, float epsilon=10.0f);

    void drawMotionDebug(Renderer& renderer);

private:
    entt::registry _registry;

    std::unordered_map<std::string, entt::entity> _nameIdMap;

    AStar::Generator* _pathGenerator = nullptr;

    Vec2 _mapSize;
    Vec2 _tileSize;

    bool _debugMode = false;
};



}