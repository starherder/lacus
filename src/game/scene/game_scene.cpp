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
    _gamePlay.update(deltaTime);
}

void GameScene::onDraw() 
{
    auto& renderer = application().renderer();

    _tileMap.draw(renderer);

    drawPathFind();

    _gamePlay.draw(renderer);
}

void GameScene::onStart()
{
    spdlog::info("========================= GameScene::onStart =========================");

    showAllGui();

    _gamePlay.destroyActor("actor1");
    _gamePlay.destroyActor("actor2");

    Vec2 destPos = {22*_tileMap.tileSize().x, 6*_tileMap.tileSize().y};

    _actor = _gamePlay.createActor("actor1");
    _gamePlay.motionStart(_actor, destPos);
    _gamePlay.motionPause(_actor, true);

    //auto actor2 = _gamePlay.createActor("actor2");
    //_gamePlay.setActorPos(actor2, destPos);
}

void GameScene::onStop()
{
    _gamePlay.motionStop(_actor);
    _gamePlay.destroyActor("actor1");

    closeAllGui();
    
    spdlog::info("========================= GameScene::onStop =========================");
}

void GameScene::showAllGui()
{
    ui::GuiManager::inst().showForm<FormMain>("form_main");
    imgui::ImFormManager::inst().setStyle(imgui::ImGuiTheme::Light);

    auto form_debug = imgui::ImFormManager::inst().showForm<ImFormDebug>("ImFormDebug");
    if(form_debug) {
        form_debug->on_show_collision_debug.connect(this, &GameScene::onShowCollisionDebug);
        form_debug->on_motion_start.connect(this, &GameScene::onMotionStart);
        form_debug->on_motion_speed_changed.connect(this, &GameScene::onMotionSpeedChanged);
    }
}

void GameScene::closeAllGui()
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

    for(auto& grid : _tileMap.collisionPoints()) 
    {
        _generator.addCollision({grid.x, grid.y});
    }

    _gamePlay.initPathFind(&_generator, _tileMap.mapSize(), _tileMap.tileSize() );

    // -------------- show collision info ------------------
    auto& tileSize = _tileMap.tileSize();
    for(auto& grid : _tileMap.collisionPoints())
    {
        _collisionRects.push_back({grid.x*tileSize.x, grid.y*tileSize.y, tileSize.x, tileSize.y});
    }
}

void GameScene::drawPathFind()
{
    if(!_showCollisionDebug)
    {
        return;
    }

    // -------------- show collision info ------------------
    auto& renderer = application().renderer();
    renderer.setDrawColor(Color{255, 0, 0, 100});
    renderer.drawFillRects(_collisionRects.data(), (int)_collisionRects.size());

    auto& mapSize = _tileMap.mapSize();
    auto& tileSize = _tileMap.tileSize();

    for(int x=0; x<=mapSize.x; ++x)
    {
        renderer.drawLine({x*tileSize.x, 0}, {x*tileSize.x, mapSize.y*tileSize.y});
    }
    for(int y=0; y<=_tileMap.mapSize().y; ++y)
    {
        renderer.drawLine({0, y*tileSize.y}, {tileSize.x*mapSize.x, y*tileSize.y});
    }
}

void GameScene::onShowCollisionDebug(bool show)
{
    _showCollisionDebug = show;
    _gamePlay.setDebugMode(show);
}

void GameScene::onMotionStart(bool run)
{
    _gamePlay.motionPause(_actor, !run);
}

void GameScene::onMotionSpeedChanged(float speed)
{
    _gamePlay.setMotionSpeed(_actor, speed);
}

} 