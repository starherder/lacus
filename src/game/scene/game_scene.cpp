#include "game_scene.h"

#include "../ui/form_main.h"
#include "../ui/imform_debug.h"

#include "glm/glm.hpp"

namespace game {


GameScene::GameScene(GameContext& context)
    : engine::Scene(context.applicaton()), _context(context)
{
    _camera.init(&application());

    _camera.setPos(Vec2{0, 0});

    _camera.setSize(Vec2{_context.window().getSize()});

    _context.dispatcher().sink<RoleCrossGrid>().connect<&GameScene::onRoleCrossGrid>(this);
}

GameScene::~GameScene()
{
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
    _camera.setPos({0, 0});

    on_load_progress.emit(0.0f);

    auto res = _tileMap.load(mapPath);
    if(!res)
    {
        return false;
    }

    on_load_progress(0.5f);

    _tileMap.bake(application().resourceManager());

    on_load_progress(0.7f);

    initPathFind();

    on_load_progress(0.8f);

    loadObjects();

    on_load_progress(1.0f);

    return true;
}

bool GameScene::unload()
{
    auto res = _tileMap.unload();
    if(!res)
    {
        return false;
    }

    unloadObjects();

    return true;
}

void GameScene::onUpdate(float deltaTime)
{
    _camera.update(deltaTime);
}

void GameScene::onDraw() 
{
    auto& renderer = application().renderer();

    _tileMap.draw(renderer, _camera);
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
    auto form_debug = imgui::ImFormManager::inst().showForm<ImFormDebug>("ImFormDebug");
    if(form_debug) 
    {
        form_debug->init(&_context);
    }
}

void GameScene::setDebugInfo(bool show)
{
    _context.setDebugMode(show);
}

void GameScene::closeAllGui()
{
    imgui::ImFormManager::inst().closeForm("ImFormDebug");
}   

void GameScene::initPathFind()
{
    auto& pathFinder = _context.pathFinder();
    pathFinder.clear();

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
            createMapActor(obj);
        }
    }
}

void GameScene::unloadObjects()
{
    _context.registry().clear();

    _gridObjects.clear();

    _collisionDebugRects.clear();

    _tileMap.unload();
}

entt::entity GameScene::selectObjectAtPos(const Vec2& pos)
{
    auto grid = getGridFromPos(pos);
    auto& objset = getObjectsInGrid(grid);
    if (objset.empty()) 
    {
        _context.dispatcher().trigger(ObjectSelection{ entt::null });
        return entt::null;
    }

    for (auto& obj : objset)
    {
        if (_context.registry().valid(obj)) 
        {
            const auto& transComp = _context.registry().get<CompTransform>(obj);
            auto rect = Rect{ transComp.position - transComp.size / 2.0f, transComp.size };
            if (rect.contains(pos)) 
            {
                spdlog::info("object ({}) selected.", (int)obj);
                _context.dispatcher().trigger(ObjectSelection{ obj });
                return obj;
            }
        }
    }

    spdlog::info("object (null) selected.");
    _context.dispatcher().trigger(ObjectSelection{ entt::null });
    return entt::null;
}

entt::entity GameScene::createMapActor(const MapObject& obj)
{
    return createActor(obj.name, obj.pos);
}

entt::entity GameScene::createActor(const std::string& cfgid, const Vec2& pos)
{
    auto ent = ObjectFactory::inst().createActor(cfgid);
    if(ent==entt::null) 
    {
        return ent;
    }

    auto* trans = _context.registry().try_get<CompTransform>(ent);
    if(trans) 
    {
        trans->position = pos;
    }

    auto* patrol = _context.registry().try_get<CompNpcPatrol>(ent);
    if(patrol) 
    {
        patrol->origin_pos = pos;
    }

    auto* btree = _context.registry().try_get<CompBehavior>(ent);
    if (btree && btree->bevtree) 
    {
        btree->bevtree->getBlackboard()->set("context", &_context);
        btree->bevtree->getBlackboard()->set("actor", ent);
    }

    auto grid = getGridFromPos(pos);
    addObjectToGrid(ent, grid);

    spdlog::info("createObject: id = {}, name = {}", (uint32_t)ent, cfgid);
    return ent;
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
}

void GameScene::addObjectToGrid(entt::entity ent, const Vec2i& grid)
{
    _gridObjects[grid].insert(ent);
}

void GameScene::removeObjectFromGrid(entt::entity ent, const Vec2i& grid)
{
    _gridObjects[grid].erase(ent);
}

const GameScene::EntitySet& GameScene::getObjectsInGrid(const Vec2i& grid)
{ 
    return _gridObjects[grid]; 
}

const std::multimap<float, Vec2i>& GameScene::getGridsInCircle(const Vec2& center, float radius)
{
    static std::multimap<float, Vec2i> grids;
    grids.clear();

    float l = center.x - radius;
    float r = center.x + radius;
    float t = center.y - radius;
    float b = center.y + radius;

    Vec2i gridLT = getGridFromPos({ l, t });
    Vec2i gridRB = getGridFromPos({ r, b });
    for (int x = gridLT.x; x <= gridRB.x; x++)
    {
        for (int y = gridLT.y; y <= gridRB.y; y++)
        {
            Vec2i grid = {x, y};
            auto gridCenter = getGridCenterPos(grid);
            float dis = glm::distance(gridCenter, center);
            if (dis <= radius)
            {
                grids.insert({dis, grid});
            }
        }
    }

    return grids;
}

const std::multimap<float, Vec2i>& GameScene::getGridsInRing(const Vec2& center, float min_radius, float max_radius)
{
    static std::multimap<float, Vec2i> result;
    result.clear();

    if (min_radius >= max_radius)
    {
        spdlog::error("getGridsInRing: min_dis({}) >= max_dis({})", min_radius, max_radius);
        return result;
    }

    auto& outerObjects = getGridsInCircle(center, max_radius);

    for (auto& [dis, grid] : outerObjects)
    {
        if (dis > min_radius)
        {
            result.insert({dis, grid});
        }
    }

    return result;
}

void GameScene::swichCoord(CompTransform& trans, CoordMode coordmode)
{
    if (trans.coord_mode == coordmode) return;

    if (coordmode == CoordMode::ScreenSpace)
    {
        trans.position = _camera.worldToScreen(trans.position);
    }
    else
    {
        trans.position = _camera.screenToWorld(trans.position);
    }

    trans.coord_mode = coordmode;
}

const std::multimap<float, entt::entity>& GameScene::getObjectsInCircle(const Vec2& center, float radius)
{
    static std::multimap<float, entt::entity> result;
    result.clear();

    if (radius == 0) 
    {
        auto grid = getGridFromPos(center);
        auto objects = getObjectsInGrid(grid);
        for (auto& obj : objects)
        {
            result.insert({0, obj});
        }
        return result;
    }

    float l = center.x - radius;
    float r = center.x + radius;
    float t = center.y - radius;
    float b = center.y + radius;

    Vec2i gridLT = getGridFromPos({ l, t });
    Vec2i gridRB = getGridFromPos({ r, b });
    for (int x = gridLT.x; x <= gridRB.x; x++)
    {
        for (int y = gridLT.y; y <= gridRB.y; y++)
        {
            Vec2i grid = { x, y };
            auto gridCenter = getGridCenterPos(grid);
            float dis = glm::distance(gridCenter, center);
            if (dis <= radius)
            {
                auto& gridobjs = getObjectsInGrid(grid);
                for (auto& obj : gridobjs)
                {
                    result.insert({ dis, obj });
                }
            }
        }
    }

    return result;
}

const std::multimap<float, entt::entity>& GameScene::getObjectsInRing(const Vec2& center, float min_radius, float max_radius)
{
    static std::multimap<float, entt::entity> result;
    result.clear();

    if (min_radius >= max_radius)
    {
        spdlog::error("getObjectsInRing: min_dis({}) >= max_dis({})", min_radius, max_radius);
        return result;
    }

    auto& outerObjects = getObjectsInCircle(center, max_radius);

    for (auto& [dis, obj] : outerObjects)
    {
        if (dis >= min_radius)
        {
            result.insert({ dis, obj });
        }
    }

    return result;
}

SkyEffect GameScene::getSkyEffect()
{
    auto views = _registry.view<CompSkyEffect>();
    for (auto ent : views)
    {
        auto& compSky = _context.registry().get<CompSkyEffect>(ent);
        return compSky.effect;
    }

    return SkyEffect::None;
}

} 