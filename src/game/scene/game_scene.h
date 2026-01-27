#pragma once
#include "engine/scene.h"
#include "tilemap/tile_map.h"

#include "game/scene/game_context.h"
#include "game/scene/object_factory.h"
#include "game/ecs/comm_event.h"
#include "game/scene/game_data.h"

#include "game_camera.h"

#include <thread>
#include <chrono>

namespace game {

    using namespace engine;


    class GameScene : public engine::Scene, public signals::SlotHandler
    {
        using EntitySet = std::set<entt::entity>;
        using GridEntityMap = std::map<Vec2i, EntitySet, Geometry::Vec2iComparator>;
        using NameEntityMap = std::unordered_map<std::string, entt::entity>;
        using MapObject = tilemap::MapObject;

    public:
        GameScene() = default;
        GameScene(const GameScene&) = delete;
        GameScene(GameScene&&) = delete;
        GameScene& operator=(const GameScene&) = delete;
        
        GameScene(GameContext& context);
        ~GameScene();

        Vec2 mapSize();
        Vec2 tileSize();

        Vec2i getGridFromPos(const Vec2& pos);
        Vec2 getGridLeftTopPos(const Vec2i& grid);
        Vec2 getGridCenterPos(const Vec2i& grid);
        Vec2 normalToGridPos(const Vec2& pos);

        void swichCoord(CompTransform& trans, CoordMode coordmode);

        bool load(const engine::fs::path& mapPath) override;
        bool unload() override;

        void onUpdate(float deltaTime) override;
        void onDraw() override;

        void onStart() override;
        void onStop() override;

        const tilemap::TileMap& mapInfo() { return _tileMap; }

        GameCamera& camera() { return _camera; }
        GameData& dataCenter() { return _gameData; }
        entt::registry& registry() { return _registry;  }

        entt::entity createMapActor(const MapObject& obj);
        entt::entity createActor(const std::string& cfgid, const Vec2& pos);

        void destroyActor(entt::entity id);

        entt::entity selectObjectAtPos(const Vec2& pos);

        void addObjectToGrid(entt::entity ent, const Vec2i& grid);
        void removeObjectFromGrid(entt::entity ent, const Vec2i& grid);
        const EntitySet& getObjectsInGrid(const Vec2i& grid);

        int getGridWalkType(const Vec2i& grid);

        const std::multimap<float, Vec2i>& getGridsInCircle(const Vec2& center, float radius);
        const std::multimap<float, Vec2i>& getGridsInRing(const Vec2& center, float min_radius, float max_radius);

        const std::multimap<float, entt::entity>& getObjectsInCircle(const Vec2& center, float radius);
        const std::multimap<float, entt::entity>& getObjectsInRing(const Vec2& center, float min_radius, float max_radius);

        void setDebugInfo(bool show);
        
        auto& getCollisionDebugRects() { return _collisionDebugRects; }

        SkyEffect getSkyEffect();

        entt::entity getSelectEntity() { return _selectEntity; }

    private:
        void showAllGui();

        void closeAllGui();

        void initPathFind();

        void loadObjects();

        void unloadObjects();

        void onRoleCrossGrid(const EvtRoleCrossGrid& e);
        void onRoleDestroyed(const EvtRoleDestroyed& e);

        void loadInThread(const engine::fs::path& mapPath);
        void unloadInThread();

        void onMouseLeftPressed(const Vec2& pos);
        void onMouseLeftRelease(const Vec2& pos);

        void onMouseLeftDrag(const Vec2& pos, const Vec2& offset);

        void onMouseLeftDragStart(const Vec2& pos);
        void onMouseLeftDragFinish(const Vec2& pos);

        void onMouseLeftClick(const Vec2& pos);
        void onMouseRightClick(const Vec2& pos);

        void moveSelectActor(const Vec2& pos);

        bool dragSelectActor(const Vec2& pos);
        bool dragSelectActorInProgress(const Vec2& pos);
        bool dropSelectActor(const Vec2& pos);
        bool canDropToPos(const Vec2& pos);

    private:
        tilemap::TileMap _tileMap;

        GameContext& _context;

        entt::registry _registry;

        GameCamera _camera;

        GameData _gameData;

        GridEntityMap _gridObjects;

        std::mutex _threadMutex;

        entt::entity _selectEntity = entt::null;

        // debug
        std::vector<Rect> _collisionDebugRects;
    };



}