#include "game_scene.h"

namespace game {


GameScene::GameScene(const engine::Application& app)
    : engine::Scene(app)
{
}

GameScene::~GameScene()
{

}

bool GameScene::load(const std::string& scenePath)
{
    auto res = _tileMap.load(scenePath);
    if(!res)
    {
        return false;
    }

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

void GameScene::onRender() 
{

}

void GameScene::onRenderUI()
{

}


} 