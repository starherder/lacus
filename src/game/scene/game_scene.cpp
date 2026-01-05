#include "game_scene.h"

#include "../ui/form_main.h"
#include "../ui/imform_debug.h"

#include "game/ecs/system_motion.h"
#include "game/ecs/system_render.h"
#include "game/ecs/system_bevtree.h"
#include "game/ecs/system_fight.h"
#include "game/ecs/system_pickup.h"
#include "game/ecs/system_dead.h"

namespace game {


GameScene::GameScene(GameContext& context)
    : engine::Scene(context.applicaton()), _context(context)
{
    _camera.setPos(Vec2{0, 0});
    _camera.setSize(Vec2{_context.window().getSize()});

    initEscSystem();
}

GameScene::~GameScene()
{
}

void GameScene::initEscSystem()
{
    _ecsSystems.insert({ EcsPriority::Middle, std::make_shared<MotionSystem>(_context) });
    _ecsSystems.insert({ EcsPriority::Middle, std::make_shared<RenderSystem>(_context) });
    _ecsSystems.insert({ EcsPriority::Middle, std::make_shared<BevTreeSystem>(_context) });
    _ecsSystems.insert({ EcsPriority::Middle, std::make_shared<PickupSystem>(_context) });
    _ecsSystems.insert({ EcsPriority::Middle, std::make_shared<DeadSystem>(_context) });

    _context.dispatcher().sink<RoleCrossGrid>().connect<&GameScene::onRoleCrossGrid>(this);
}

Vec2 GameScene::mapSize()
{
    return _tileMap.mapSize();
}

Vec2 GameScene::tileSize()
{
    return _tileMap.tileSize();
}

Vec2i GameScene::getGridFromPos(const Vec2& pos)
{
    Vec2 gridf = pos / tileSize();
    Vec2i grid = { (int)std::ceil(gridf.x) - 1, (int)std::ceil(gridf.y) - 1 };
    return grid;
}

Vec2 GameScene::getGridLeftTopPos(const Vec2i& grid)
{
    return { grid.x * tileSize().x, grid.y * tileSize().y };
}

Vec2 GameScene::getGridCenterPos(const Vec2i& grid)
{
    return { (grid.x + 0.5f) * tileSize().x, (grid.y + 0.5f) * tileSize().y };
}

bool GameScene::load(const engine::fs::path& mapPath)
{
    auto res = _tileMap.load(mapPath);
    if(!res)
    {
        return false;
    }

    _camera.init(&application());

    _tileMap.bake(application().resourceManager());

    initPathFind();

    loadObjects();

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
    _camera.update(deltaTime);

    for (auto& [prio, sys] : _ecsSystems) 
    {
        sys->update(deltaTime);
    }
}

void GameScene::onDraw() 
{
    auto& renderer = application().renderer();

    _tileMap.draw(renderer, _camera);

    for (auto& [prio, sys] : _ecsSystems)
    {
        sys->draw();
    }

    drawDebugView();
}

void GameScene::onStart()
{
    spdlog::info("========================= GameScene::onStart =========================");

    showAllGui();
}

void GameScene::onStop()
{
    closeAllGui();
    
    spdlog::info("========================= GameScene::onStop =========================");
}

void GameScene::showAllGui()
{
    //ui::GuiManager::inst().showForm<FormMain>("form_main");

    auto form_debug = imgui::ImFormManager::inst().showForm<ImFormDebug>("ImFormDebug");
    if(form_debug) 
    {
        form_debug->on_show_debug.connect(this, &GameScene::onShowDebugInfo);
    }
}

void GameScene::onShowDebugInfo(bool show)
{
    _context.setDebugMode(show);
}

void GameScene::closeAllGui()
{
    ui::GuiManager::inst().closeForm("form_main");
    imgui::ImFormManager::inst().closeForm("ImFormDebug");
}   

void GameScene::initPathFind()
{
    auto& pathFinder = _context.pathFinder();
    pathFinder.setWorldSize(_tileMap.mapSize());
    pathFinder.setHeuristic(AStar::Heuristic::euclidean);
    pathFinder.setDiagonalMovement(false);

    for(auto& grid : _tileMap.collisionPoints()) 
    {
        pathFinder.addCollision(grid);
    }

    // -------------- show collision info ------------------
    auto& tileSize = _tileMap.tileSize();
    for(auto& grid : _tileMap.collisionPoints())
    {
        _collisionDebugRects.push_back({grid.x*tileSize.x, grid.y*tileSize.y, tileSize.x, tileSize.y});
    }
}

void GameScene::loadObjects()
{
    auto layer = _tileMap.getObjectLayer();
    if (layer) {
        for (auto& [id, obj] : layer->objects) {
            createObject(obj);
        }
    }
}

void GameScene::drawDebugView()
{
    if(!_context.debugMode())
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

bool GameScene::createObject(const MapObject& obj)
{
    auto ent = ObjectFactory::inst().createRole(obj.name);
    if(ent==entt::null) 
    {
        return false;
    }

    auto* trans = _context.registry().try_get<CompTransform>(ent);
    if(trans) 
    {
        trans->position = obj.pos;
    }

    auto* patrol = _context.registry().try_get<CompNpcPatrol>(ent);
    if(patrol) 
    {
        patrol->origin_pos = obj.pos;
    }

    auto* btree = _context.registry().try_get<CompBevtree>(ent);
    if (btree && btree->bevtree) 
    {
        btree->bevtree->getBlackboard()->set("context", &_context);
        btree->bevtree->getBlackboard()->set("actor", ent);
    }

    auto grid = getGridFromPos(obj.pos);
    addObjectToGrid(ent, grid);

    spdlog::info("createObject: id = {}, name = {}", (int)ent, obj.name);
    return true;
}

entt::entity GameScene::getActor(const std::string& name)
{
    auto it = _nameIdMap.find(name);
    if (it != _nameIdMap.end())
    {
        return it->second;
    }

    return entt::null;
}

void GameScene::destroyActor(entt::entity id)
{
    if (!_registry.valid(id))
    {
        spdlog::warn("entity {} not exist.", (int32_t)id);
        return;
    }
    _registry.destroy(id);
}

void GameScene::destroyActor(const std::string& name)
{
    auto id = getActor(name);
    if (id != entt::null)
    {
        destroyActor(id);
    }
}

void GameScene::destroyAllActor()
{
    _registry.clear();
}

void GameScene::onRoleCrossGrid(const RoleCrossGrid& e)
{
    auto& lstset = _gridObjects[e.lst_grid];
    lstset.erase(e.actor);

    auto& curset = _gridObjects[e.cur_grid];
    curset.insert(e.actor);

    //spdlog::info("onRoleCrossGrid: lst_grid({},{}).size = {}, cur_grid({},{}).size = {}", 
    //    e.lst_grid.x, e.lst_grid.y, lstset.size(), e.cur_grid.x, e.cur_grid.y, curset.size());
}

void GameScene::addObjectToGrid(entt::entity ent, const Vec2i& grid)
{
    _gridObjects[grid].insert(ent);

    //spdlog::info("addObjectToGrid: cur_grid({},{}).size = {}",
    //    grid.x, grid.y, _gridObjects[grid].size());
}

const GameScene::EntitySet& GameScene::getObjectsInGrid(const Vec2i& grid)
{ 
    return _gridObjects[grid]; 
}

} 