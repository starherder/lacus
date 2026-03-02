#pragma once

#include "quadtree/quad_tree.h"

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
        signals::Signal<entt::entity> on_select_object;
        signals::Signal<entt::entity> on_unselect_object;

    public:
        GameScene() = default;
        GameScene(const GameScene&) = delete;
        GameScene(GameScene&&) = delete;
        GameScene& operator=(const GameScene&) = delete;
        
        GameScene(GameContext& context);
        ~GameScene();

        Vec2 sceneSize();
        Vec2 scenePos();

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

        entt::entity createObjectInScene(const std::string& cfgid, const Vec2& pos, CampSide side);
        void destroyObject(entt::entity id);

        void addObjectToScene(entt::entity id);
        void removeObjectFromScene(entt::entity id);

        size_t sceneObjectCount() { return _sceneObjects.size(); }

        Vec2i getObjectGrid(entt::entity id);
        Vec2 getObjectPos(entt::entity id);
        void setObjectPos(entt::entity id, const Vec2& pos);

        entt::entity findObjectAtPos(const Vec2& pos);
        EntityVector getObjectsInGrid(const Vec2i& grid);
        const EntityDisMap& getObjectsInCircle(const Vec2& center, float radius);

        int getGridWalkType(const Vec2i& grid);

        SkyEffect getSkyEffect();

        entt::entity getSelectEntity() { return _selectEntity; }

        bool objectDragable() { return _objectDragable; }
        void setObjectDragable(bool dragable) { _objectDragable = dragable; }

        void setDebugInfo(bool show);

        auto& getCollisionDebugRects() { return _collisionDebugRects; }

    private:
        void initPathFind();
        void initQuadTree();

        void loadObjects();
        void unloadObjects();

        void onRoleCrossGrid(const EvtRoleCrossGrid& e);
        void onRoleDestroyed(const EvtRoleDestroyed& e);
        void onRoleMotionStop(const EvtMotionStop& e);

        void onRoleSelect(const EvtObjectSelection& e);
        void onRoleUnselect(const EvtObjectUnselect& e);

        void loadInThread(const engine::fs::path& mapPath);
        void unloadInThread();

        void onMouseLeftPressed(const Vec2& pos);
        void onMouseLeftRelease(const Vec2& pos);
        void onMouseLeftDrag(const Vec2& pos, const Vec2& offset);
        void onMouseLeftDragStart(const Vec2& pos);
        void onMouseLeftDragFinish(const Vec2& pos);
        void onMouseLeftClick(const Vec2& pos);
        void onMouseRightClick(const Vec2& pos);
        void onMouseMotion(const Vec2& pos, const Vec2& offset);

        void onKeyDown(KeyCode key);

        void onHoverObject(entt::entity obj);
        void onLeaveObject(entt::entity obj);

        void moveSelectActor(const Vec2& pos);

        bool dragSelectActor(const Vec2& pos);
        bool dragSelectActorInProgress(const Vec2& pos);
        bool dropSelectActor(const Vec2& pos);
        bool canDropToPos(const Vec2& pos);

        void onMoveStep(const Vec2i& dir);
        void onSkipMove();

    private:
        tilemap::TileMap _tileMap;

        GameContext& _context;

        entt::registry _registry;

        GameCamera _camera;

        GameData _gameData;;

        entt::entity _selectEntity = entt::null;
        entt::entity _hoverEntity = entt::null;

        // 场景对象：在场景中有位置、碰撞、会加入到四叉树中的对象
        std::set<entt::entity> _sceneObjects;

        QuadTreePtr _quadtree = nullptr;

        bool _objectDragable = false;

        // debug
        std::vector<Rect> _collisionDebugRects;
    };



}