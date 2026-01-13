#pragma once
#include "engine/scene.h"
#include "tilemap/tile_map.h"

#include "game/scene/game_context.h"
#include "game/scene/object_factory.h"

#include "game/ecs/comm_comp.h"
#include "game/ecs/comm_system.h"
#include "game/ecs/comm_event.h"

#include "game_camera.h"


namespace game {

    using namespace engine;


    struct Vec2Compare{
        bool operator () (const Vec2i& lval, const Vec2i rval) const {
            if (lval.x == rval.x) return lval.y < rval.y;
            return lval.x < rval.x;
        }
    };


    class GameScene : public engine::Scene, public signals::SlotHandler
    {
        using EntitySet = std::set<entt::entity>;
        using GridEntityMap = std::map<Vec2i, EntitySet, Vec2Compare>;
        using NameEntityMap = std::unordered_map<std::string, entt::entity>;
        using EcsSystemMap = std::multimap<EcsPriority, std::shared_ptr<EcsSystem>>;
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

        void swichCoord(CompTransform& trans, CoordMode coordmode);

        bool load(const engine::fs::path& mapPath) override;
        bool unload() override;

        void onUpdate(float deltaTime) override;
        void onDraw() override;

        void onStart() override;
        void onStop() override;

        const tilemap::TileMap& mapInfo() { return _tileMap; }

        GameCamera& camera() { return _camera; }

        entt::registry& registry() { return _registry;  }

        entt::entity getActor(const std::string& name);

        void destroyActor(entt::entity id);
        void destroyActor(const std::string& name);
        void destroyAllActor();

        entt::entity selectObjectAtPos(const Vec2& pos);

        bool createObject(const MapObject& obj);
        bool createObject(const std::string& cfgid, const Vec2& pos);

        void addObjectToGrid(entt::entity ent, const Vec2i& grid);
        void removeObjectFromGrid(entt::entity ent, const Vec2i& grid);
        const EntitySet& getObjectsInGrid(const Vec2i& grid);

        const std::multimap<float, Vec2i>& getGridsInCircle(const Vec2& center, float radius);
        const std::multimap<float, Vec2i>& getGridsInRing(const Vec2& center, float min_radius, float max_radius);

        const std::multimap<float, entt::entity>& getObjectsInCircle(const Vec2& center, float radius);
        const std::multimap<float, entt::entity>& getObjectsInRing(const Vec2& center, float min_radius, float max_radius);

        void setDebugInfo(bool show);
        auto& getCollisionDebugRects() { return _collisionDebugRects; }

        SkyEffect getSkyEffect();

    private:
        void showAllGui();

        void closeAllGui();

        void initEscSystem();

        void initPathFind();

        void loadObjects();

        void onRoleCrossGrid(const RoleCrossGrid& e);


    private:
        tilemap::TileMap _tileMap;

        GameContext& _context;

        entt::registry _registry;

        GameCamera _camera;

        NameEntityMap _nameIdMap;

        EcsSystemMap _ecsSystems;

        GridEntityMap _gridObjects;

        // debug
        std::vector<Rect> _collisionDebugRects;
    };



}