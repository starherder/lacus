#include "game_scene.h"

#include "../ui/form_main.h"
#include "../ui/imform_debug.h"

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

    _tileMap.bake(application().resourceManager());
    
    initPathFind();

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

    if(_showCollisionDebug)
    {
        drawPathFind();
    }
}

void GameScene::onEnable()
{
    ui::GuiManager::inst().showForm<FormMain>("form_main");

    imgui::ImFormManager::inst().setStyle(imgui::ImGuiTheme::Light);

    auto form_debug = imgui::ImFormManager::inst().showForm<ImFormDebug>("ImFormDebug");
    if(form_debug)
    {
        form_debug->on_show_collision_debug.connect(this, &GameScene::onShowCollisionDebug);
    }
}

void GameScene::onDisable()
{
    ui::GuiManager::inst().closeForm("form_main");

    imgui::ImFormManager::inst().closeForm("ImFormDebug");
}

void GameScene::initPathFind()
{
    auto& mapSize = _tileMap.mapSize();

    _generator.setWorldSize({mapSize.x, mapSize.y});
    _generator.setHeuristic(AStar::Heuristic::euclidean);
    _generator.setDiagonalMovement(true); 

    for (auto& grid : _tileMap.collisionPoints()) 
    {
        _generator.addCollision({grid.x, grid.y});
    }

    Vec2i source = {2, 2};
    Vec2i target = {36, 2};
    auto& tileSize = _tileMap.tileSize();

    for(auto& grid : _tileMap.collisionPoints())
    {
        _collisionRects.push_back({grid.x*tileSize.x, grid.y*tileSize.y, tileSize.x, tileSize.y});
    }

    auto result_path = _generator.findPath({ source.x, source.y }, { target.x, target.y });
    for (auto& grid : result_path) 
    {
        _test_path.push_back({grid.x*tileSize.x, grid.y*tileSize.y, tileSize.x, tileSize.y});
    }
}

void GameScene::drawPathFind()
{
    for (auto& grid : _test_path) 
    {
        application().renderer().setDrawColor(Color{255, 0, 0, 100});
        application().renderer().drawFillRects(_collisionRects.data(), (int)_collisionRects.size());
    }

    for (auto& grid : _test_path) 
    {
        application().renderer().setDrawColor(Color{0.0f, 1.0f, 0.0f, 0.5f});
        application().renderer().drawFillRects(_test_path.data(), (int)_test_path.size());
    }
}

void GameScene::onShowCollisionDebug(bool show)
{
    _showCollisionDebug = show;
}

} 