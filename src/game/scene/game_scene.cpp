#include "game_scene.h"

#include "../ui/form_main.h"
#include "../ui/imform_debug.h"

namespace game {


GameScene::GameScene(engine::Application& app)
    : engine::Scene(app)
{
    _camera.setPos(Vec2{0, 0});
    _camera.setSize(Vec2{app.window().getSize()});
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

    _camera.init(&application());
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

    _camera.update(deltaTime);
}

void GameScene::onDraw() 
{
    auto& renderer = application().renderer();

    _tileMap.draw(renderer, _camera);

    drawDebugView();

    _gamePlay.draw(renderer, _camera);
}

void GameScene::onStart()
{
    spdlog::info("========================= GameScene::onStart =========================");

    showAllGui();
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
    //ui::GuiManager::inst().showForm<FormMain>("form_main");

    imgui::ImFormManager::inst().setStyle(imgui::ImGuiTheme::Light);

    auto form_debug = imgui::ImFormManager::inst().showForm<ImFormDebug>("ImFormDebug");
    if(form_debug) 
    {
        form_debug->on_show_collision_debug.connect(this, &GameScene::onShowCollisionDebug);
        form_debug->on_motion_pause.connect(this, &GameScene::onMotionPause);
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

    _generator.setWorldSize(mapSize);
    _generator.setHeuristic(AStar::Heuristic::euclidean);
    _generator.setDiagonalMovement(false); 

    for(auto& grid : _tileMap.collisionPoints()) 
    {
        _generator.addCollision(grid);
    }

    _gamePlay.initPathFind(&_generator, _tileMap.mapSize(), _tileMap.tileSize() );

    // -------------- show collision info ------------------
    auto& tileSize = _tileMap.tileSize();
    for(auto& grid : _tileMap.collisionPoints())
    {
        _collisionDebugRects.push_back({grid.x*tileSize.x, grid.y*tileSize.y, tileSize.x, tileSize.y});
    }
}

void GameScene::drawDebugView()
{
    if(!_showCollisionDebug)
    {
        return;
    }

    // -------------- show collision info ------------------
    static std::vector<Rect> rects;
    rects.clear();
    rects.reserve(_collisionDebugRects.size());
    rects.insert(rects.begin(), _collisionDebugRects.begin(), _collisionDebugRects.end());
    _camera.projectRects(rects.data(), (int)rects.size());

    auto& renderer = application().renderer();
    renderer.setDrawColor(Color{255, 0, 0, 100});
    renderer.drawFillRects(rects.data(), (int)rects.size());

    // -------------- show grids ------------------
    auto& mapSize = _tileMap.mapSize();
    auto& tileSize = _tileMap.tileSize();
    for(int x=0; x<=mapSize.x; ++x)
    {
        auto srcPos = _camera.projectPoint({x*tileSize.x, 0});
        auto dstPos = _camera.projectPoint({x*tileSize.x, mapSize.y*tileSize.y});
        renderer.drawLine(srcPos, dstPos);
    }
    for(int y=0; y<=_tileMap.mapSize().y; ++y)
    {
        auto srcPos = _camera.projectPoint({0, y*tileSize.y});
        auto dstPos = _camera.projectPoint({tileSize.x*mapSize.x, y*tileSize.y});
        renderer.drawLine(srcPos, dstPos);
    }
}

void GameScene::onShowCollisionDebug(bool show)
{
    _showCollisionDebug = show;
    _gamePlay.setDebugMode(show);
}

void GameScene::onMotionStart(bool start, float speed)
{
    auto& tileSize = _tileMap.tileSize();
    Vec2 srcPos = {5*tileSize.x, 5*tileSize.y};
    Vec2 destPos = {22*_tileMap.tileSize().x, 6*_tileMap.tileSize().y};

    if(start)
    {
        if(_actor == entt::null){
            _actor = _gamePlay.createActor("actor1", srcPos, tileSize);
        }

        _gamePlay.setMotionSpeed(_actor, speed);
        _gamePlay.motionStart(_actor, destPos);
    }
    else
    {
        _gamePlay.motionStop(_actor);
        _gamePlay.setActorPos(_actor, srcPos);
    }
}

void GameScene::onMotionPause(bool pause)
{
    _gamePlay.motionPause(_actor, pause);
}

void GameScene::onMotionSpeedChanged(float speed)
{
    _gamePlay.setMotionSpeed(_actor, speed);
}

} 