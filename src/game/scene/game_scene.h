#pragma once
#include "engine/scene.h"
#include "tilemap/tile_map.h"

#include "game/scene/game_context.h"

#include "game/ecs/comm_comp.h"
#include "game/ecs/comm_system.h"
#include "game/ecs/comm_event.h"

#include "game_camera.h"


namespace game {

    using namespace engine;


    class GameScene : public engine::Scene, public signals::SlotHandler
    {
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

        bool load(const engine::fs::path& mapPath) override;
        bool unload() override;

        void onUpdate(float deltaTime) override;
        void onDraw() override;

        void onStart() override;
        void onStop() override;

        GameCamera& camera() { return _camera; }
        entt::registry& registry() { return _registry;  }

        entt::entity createActor(const std::string& name, const Vec2& pos, const Vec2& size = { 64, 64 });
        entt::entity getActor(const std::string& name);

        void destroyActor(entt::entity id);
        void destroyActor(const std::string& name);
        void destroyAllActor();

        bool createObject(const MapObject& obj);

    private:
        void showAllGui();
        void closeAllGui();

        void initEscSystem();

        void initPathFind();

        void loadObjects();

        void drawDebugView();

        void onShowDebugInfo(bool show);

    private:
        tilemap::TileMap _tileMap;

        GameContext& _context;

        entt::registry _registry;

        GameCamera _camera;

        NameEntityMap _nameIdMap;

        EcsSystemMap _ecsSystems;

        // debug
        std::vector<Rect> _collisionDebugRects;
    };



}