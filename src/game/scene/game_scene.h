#pragma once

#include "quadtree/quad_tree.h"

#include "engine/scene.h"
#include "tilemap/tile_map.h"

#include "game/scene/game_context.h"
#include "game/scene/object_manager.h"
#include "game/ecs/comm_event.h"
#include "game/scene/game_data.h"
#include "game/logic/game_play.h"
#include "game/scene/scene_config.h"

#include "game_camera.h"

#include <thread>
#include <chrono>

namespace game {

    using namespace engine;

    class GameScene : public engine::Scene, public signals::SlotHandler
    {
        using EntitySet = std::set<entt::entity>;
        using EntityVector = std::vector<entt::entity>;
        using EntityDisMap = std::multimap<float, entt::entity>;

        using GridEntityMap = std::map<Vec2i, EntitySet, Geometry::Vec2iComparator>;
        using NameEntityMap = std::unordered_map<std::string, entt::entity>;
        using MapObject = tilemap::MapObject;

        using BoxType = quadtree::Box<float>;
        using QuadTreeType = quadtree::Quadtree<entt::entity, std::function<BoxType(const entt::entity&)> >;
        using QuadTreePtr = std::unique_ptr<QuadTreeType>;

    public:
        signals::Signal<entt::entity> on_hover_object;
        signals::Signal<entt::entity> on_leave_object;

    public:
        GameScene() = default;
        GameScene(const GameScene&) = delete;
        GameScene(GameScene&&) = delete;
        GameScene& operator=(const GameScene&) = delete;
        
        GameScene(GameContext& context);
        ~GameScene();

        bool load(const std::string& id) override;
        bool unload() override;

        Vec2 sceneSize();
        Vec2 scenePos();

        Vec2 mapSize();
        Vec2 tileSize();

        Vec2i getGridFromPos(const Vec2& pos);
        Vec2 getGridLeftTopPos(const Vec2i& grid);
        Vec2 getGridCenterPos(const Vec2i& grid);
        Vec2i getObjectGridSize(const CompTransform& trans);
        Vec2i getObjectLeftTopGrid(const CompTransform& trans);
        std::vector<Vec2i> getObjectGrids(const CompTransform& trans);
        std::vector<Vec2i> getObjectMoveEnterGrids(const CompTransform& trans, const Vec2i& dir);
        Rect getGridAlignedAABB(const CompTransform& trans);
        Rect getDisplayAABB(const CompTransform& trans);
        Vec2 normalToGridPos(const Vec2& pos);
        void normalToGridPos(entt::entity ent);

        void swichCoord(CompTransform& trans, CoordMode coordmode);


        void onUpdate(float deltaTime) override;
        void onDraw() override;

        void onStart() override;
        void onStop() override;

        const tilemap::TileMap& mapInfo() { return _tileMap; }

        GameCamera& camera() { return _camera; }
        GameData& dataCenter() { return _gameData; }
        entt::registry& registry() { return _registry;  }

        entt::entity createObjectInScene(const std::string& cfgid, const Vec2& pos, CampSide side);
        void destroyObject(entt::entity id);

        void addObjectToScene(entt::entity id);
        void removeObjectFromScene(entt::entity id);

        size_t sceneObjectCount() { return _sceneObjects.size(); }

        Vec2i getObjectGrid(entt::entity id);
        Vec2 getObjectPos(entt::entity id);
        void setObjectPos(entt::entity id, const Vec2& pos);

        entt::entity findObjectAtPos(const Vec2& pos);
        EntitySet getObjectsInRect(const Rect& rect);
        EntitySet getObjectsInGrid(const Vec2i& grid);
        bool hasObjectInGrid(const Vec2i& grid, ObjectType type=ObjectType::All);
        entt::entity getOneObjectInGrid(const Vec2i& grid, ObjectType type=ObjectType::All);
        const EntityDisMap& getObjectsInCircle(const Vec2& center, float radius);

        int getGridWalkType(const Vec2i& grid);

        SkyEffect getSkyEffect();

        void setDebugInfo(bool show);

        auto& getCollisionDebugRects() { return _collisionDebugRects; }

        void drawQuadTree();
        
    private:
        void initPathFind();
        void initQuadTree();

        void loadObjects();
        void unloadObjects();

        void onRoleCrossGrid(const EvtRoleCrossGrid& e);
        void onRoleDestroyed(const EvtRoleDestroyed& e);
        void onRoleMotionStop(const EvtMotionStop& e);

        void loadInThread(const MapConfig& mapPath);
        void unloadInThread();

        void onMouseMotion(const Vec2& pos, const Vec2& offset);

        void onHoverObject(entt::entity obj);
        void onLeaveObject(entt::entity obj);

        void drawQuadNode(QuadTreeType::Node* node);
        
        
    private:
        tilemap::TileMap _tileMap;

        GameContext& _context;

        entt::registry _registry;

        GameCamera _camera;

        GameData _gameData;;

        entt::entity _hoverEntity = entt::null;

        // 场景对象：在场景中有位置、碰撞、会加入到四叉树中的对象
        std::set<entt::entity> _sceneObjects;

        QuadTreePtr _quadtree = nullptr;

        std::unique_ptr<GamePlay> _gamePlay = nullptr;

        // debug
        std::vector<Rect> _collisionDebugRects;
    };



}