#pragma once
#include "game/scene/tile_map.h"
#include "engine/scene.h"

namespace game {

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

    private:
        TileMap _tileMap;
    };



}