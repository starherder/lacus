#pragma once
#include "engine/scene.h"
#include "a_star/a_star.hpp"

#include "tilemap/tile_map.h"
#include "game/play/game_play.h"
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
        
        GameScene(engine::Application& app);
        ~GameScene();

        bool load(const engine::fs::path& mapPath) override;
        bool unload() override;

        void onUpdate(float deltaTime) override;
        void onDraw() override;

        void onStart() override;
        void onStop() override;

    private:
        void showAllGui();
        void closeAllGui();

        void initPathFind();
        void drawDebugView();

        void onMotionStart(bool start, float speed);
        void onMotionPause(bool pause);

        void onShowCollisionDebug(bool show);
        void onMotionSpeedChanged(float speed);

    private:
        tilemap::TileMap _tileMap;

        AStar::Generator _generator;

        GamePlay _gamePlay;

        GameCamera _camera;

        // test
        entt::entity _actor = entt::null;

        // debug
        bool _showCollisionDebug = false;
        std::vector<Rect> _collisionDebugRects;
    };



}