#pragma once
#include "engine/scene.h"
#include "game/scene/tile_map.h"
#include "a_star/a_star.hpp"

namespace game {

    using namespace engine;

    class GameScene : public engine::Scene
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

        void onEnable();
        void onDisable();

    private:
        void initPathFind();
        void drawPathFind();

    private:
        TileMap _tileMap;

        std::vector<Rect> _collisionRects;
        std::vector<Rect> _test_path;
        
        AStar::Generator _generator;
    };



}