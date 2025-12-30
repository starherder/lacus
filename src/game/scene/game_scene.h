#pragma once
#include "engine/scene.h"
#include "tilemap/tile_map.h"

#include "game/play/comp_common.h"
#include "game/play/game_context.h"
#include "game/play/ecs_system.h"

#include "game_camera.h"


namespace game {

    using namespace engine;

    class GameScene : public engine::Scene, public signals::SlotHandler
    {
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

        ActorState getActorState(entt::entity id);

        void destroyActor(entt::entity id);
        void destroyActor(const std::string& name);
        void destroyAllActor();

    private:
        void showAllGui();
        void closeAllGui();

        void initEscSystem();

        void initPathFind();
        void drawDebugView();

        void onShowDebugInfo(bool show);

    private:
        tilemap::TileMap _tileMap;

        GameContext& _context;

        entt::registry _registry;

        GameCamera _camera;

        std::unordered_map<std::string, entt::entity> _nameIdMap;

        std::multimap<EcsPriority, std::shared_ptr<EcsSystem>> _ecsSystems;

        // debug
        std::vector<Rect> _collisionDebugRects;
    };



}