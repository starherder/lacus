#include "game_scene.h"

namespace game {


GameScene::GameScene(engine::Application& app)
    : engine::Scene(app)
{
}

GameScene::~GameScene()
{
}

bool GameScene::load(const engine::fs::path& mapPath)
{
    auto res = _tileMap.load(mapPath);
    if(!res)
    {
        return false;
    }

    _tileMap.bakeGeometry(application().resourceManager());

    return true;
}

bool GameScene::unload()
{
    auto res = _tileMap.unload();
    if(!res)
    {
        return false;
    }

    return true;
}

void GameScene::onUpdate(float deltaTime)
{
}

void GameScene::onDraw() 
{
    _tileMap.draw(application().renderer());
}


} 