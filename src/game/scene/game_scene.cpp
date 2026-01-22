#include "game_scene.h"

namespace game 
{

GameScene::GameScene(GameContext& context)
    : engine::Scene(context.applicaton()), _context(context)
{
    _camera.init(&application());

    _camera.setPos(Vec2{0, 0});

    _camera.setSize(Vec2{_context.window().getSize()});

    _context.dispatcher().sink<EvtRoleCrossGrid>().connect<&GameScene::onRoleCrossGrid>(this);
    _context.dispatcher().sink<EvtRoleDestroyed>().connect<&GameScene::onRoleDestroyed>(this);

    _context.eventDispatcher().onMouseLeftDown.connect(this, &GameScene::onMouseLeftPressed, -1);
    _context.eventDispatcher().onMouseLeftDrag.connect(this, &GameScene::onMouseLeftDrag, -1);

    _context.eventDispatcher().onMouseLeftDragStart.connect(this, &GameScene::onMouseLeftDragStart, -1);
    _context.eventDispatcher().onMouseLeftDragFinish.connect(this, &GameScene::onMouseLeftDragFinish, -1);

    _context.eventDispatcher().onMouseLeftClicked.connect(this, &GameScene::onMouseLeftClick, -1);
    _context.eventDispatcher().onMouseRightClicked.connect(this, &GameScene::onMouseRightClick, -1);
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

Vec2 GameScene::normalToGridPos(const Vec2& pos)
{
    return getGridCenterPos(getGridFromPos(pos));
}

bool GameScene::load(const engine::fs::path& mapPath)
{
    _camera.setPos({ 0, 0 });

    std::thread loadthread([this, mapPath]() {
        loadInThread(mapPath);
    });

    loadthread.detach();
    return true;
}

bool GameScene::unload()
{
    _ready = false;

    std::thread unloadthread([this]() {
        unloadInThread();
    });

    unloadthread.detach();
    return true;
}

void GameScene::loadInThread(const engine::fs::path& mapPath)
{
    std::lock_guard<std::mutex> lock(_threadMutex);

    on_load_progress.emit(0.0f);

    // TODO: remove this
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    on_load_progress(0.2f);

    auto res = _tileMap.load(mapPath);
    if(!res)
    {
        return;
    }

    // TODO: remove this
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    on_load_progress(0.5f);

    _tileMap.bake(application().resourceManager());

    // TODO: remove this
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    on_load_progress(0.7f);

    initPathFind();

    // TODO: remove this
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    on_load_progress(0.8f);

    loadObjects();

    // TODO: remove this
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    on_load_progress(1.0f);

    _ready = true;
}

void GameScene::unloadInThread()
{
    std::lock_guard<std::mutex> lock(_threadMutex);

    auto res = _tileMap.unload();
    if(!res)
    {
        return ;
    }

    unloadObjects();
}

void GameScene::onUpdate(float deltaTime)
{
    if (!_ready) return;

    _camera.update(deltaTime);
}

void GameScene::onDraw() 
{
    if (!_ready) return;

    auto& renderer = application().renderer();

    _tileMap.draw(renderer, _camera);
}

void GameScene::onStart()
{
    SPDLOG_INFO("========================= GameScene::onStart =========================");

    showAllGui();
}

void GameScene::onStop()
{
    closeAllGui();
    
    SPDLOG_INFO("========================= GameScene::onStop =========================");
}

void GameScene::showAllGui()
{
}

void GameScene::setDebugInfo(bool show)
{
    _context.setDebugMode(show);
}

void GameScene::closeAllGui()
{
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
        _context.dispatcher().trigger(EvtObjectSelection{ entt::null });
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
                SPDLOG_INFO("object ({}) selected.", (int)obj);
                _context.dispatcher().trigger(EvtObjectSelection{ obj });
                return obj;
            }
        }
    }

    //SPDLOG_INFO("object (null) selected.");
    _context.dispatcher().trigger(EvtObjectSelection{ entt::null });
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

    //SPDLOG_INFO("createObject: id = {}, name = {}", (uint32_t)ent, cfgid);
    return ent;
}

void GameScene::destroyActor(entt::entity id)
{
    if (!_registry.valid(id))
    {
        SPDLOG_WARN("entity {} not exist.", (int32_t)id);
        return;
    }

    //_registry.destroy(id);
    _registry.emplace_or_replace<CompDestroy>(id);
}

void GameScene::onRoleDestroyed(const EvtRoleDestroyed& e)
{
    auto pTrans = _context.registry().try_get<CompTransform>(e.actor);
    if(pTrans)
    {
        auto grid = getGridFromPos(pTrans->position);

        auto& objset = _gridObjects[grid];
        objset.erase(e.actor);
    }
}

void GameScene::onRoleCrossGrid(const EvtRoleCrossGrid& e)
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

int GameScene::getGridWalkType(const Vec2i& grid)
{
    if (grid.x < 0 || grid.x > _tileMap.mapSize().x 
        || grid.y<0 || grid.y > _tileMap.mapSize().y)
    {
        return (int)tilemap::WalkType::Collision;
    }

    auto optType = mapInfo().getTileProperty<int>(grid.x, grid.y, "walktype");
    if (!optType) return (int)tilemap::WalkType::Collision;

    return optType.value();
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
        SPDLOG_ERROR("getGridsInRing: min_dis({}) >= max_dis({})", min_radius, max_radius);
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
        SPDLOG_ERROR("getObjectsInRing: min_dis({}) >= max_dis({})", min_radius, max_radius);
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

void GameScene::onMouseLeftPressed(const Vec2& pos)
{
    auto scenePos = camera().screenToWorld(pos);
    _selectEntity = selectObjectAtPos(scenePos);
}

void GameScene::onMouseLeftRelease(const Vec2& pos)
{
}

void GameScene::onMouseLeftDrag(const Vec2& pos, const Vec2& offset)
{
    if (dragSelectActorInProgress(pos))
    {
        slot_context().setBreak(true);
    }
}

void GameScene::onMouseLeftDragStart(const Vec2& pos)
{
    if (dragSelectActor(pos))
    {
        slot_context().setBreak(true);
    }
}

void GameScene::onMouseLeftDragFinish(const Vec2& pos)
{
    if (dropSelectActor(pos))
    {
        slot_context().setBreak(true);
    }
}

void GameScene::onMouseLeftClick(const Vec2& pos)
{
}

void GameScene::onMouseRightClick(const Vec2& pos)
{
    if (_context.registry().valid(_selectEntity))
    {
        auto scenePos = camera().screenToWorld(pos);
        moveSelectActor(scenePos);
    }

}

void GameScene::moveSelectActor(const Vec2& pos)
{
    if (_context.registry().valid(_selectEntity) == false)
    {
        return;
    }

    auto bevComp = _context.registry().try_get<CompBehavior>(_selectEntity);
    if (bevComp && bevComp->bevtree)
    {
        bevComp->bevtree->stop();
    }

    if (_context.registry().try_get<CompMotion>(_selectEntity))
    {
        auto gridPos = getGridFromPos(pos);
        _context.dispatcher().trigger(EvtMoveToGrid{ _selectEntity, gridPos, true });
    }
}

bool GameScene::dragSelectActor(const Vec2& pos)
{
    if (!_context.registry().valid(_selectEntity))
    {
        return false;
    }

    auto pDead = _context.registry().try_get<CompDead>(_selectEntity);
    if (pDead)
    {
        return false;
    }

    auto pTrans = _context.registry().try_get<CompTransform>(_selectEntity);
    if (!pTrans)
    {
        return false;
    }
    
    auto dstpos = pTrans->position;
    _context.registry().emplace_or_replace<CompDragging>(_selectEntity, CompDragging{ dstpos, dstpos, pTrans->size });

    removeObjectFromGrid(_selectEntity, getGridFromPos(pTrans->position));

    auto bevComp = _context.registry().try_get<CompBehavior>(_selectEntity);
    if (bevComp && bevComp->bevtree)
    {
        bevComp->bevtree->stop();
    }

    return true;
}

bool GameScene::dragSelectActorInProgress(const Vec2& pos)
{
    if (!_context.registry().valid(_selectEntity))
    {
        return false;
    }

    auto pDragging = _context.registry().try_get<CompDragging>(_selectEntity);
    if (!pDragging)
    {
        return false;
    }

    auto pTrans = _context.registry().try_get<CompTransform>(_selectEntity);
    if (!pTrans)
    {
        return false;
    }

    auto dstpos = camera().screenToWorld(pos);
    if (!canDropToPos(dstpos))
    {
        pDragging->ground_color = _context.gameConfig().display.ground_color_drag_error;
        pDragging->border_color = _context.gameConfig().display.border_color_drag_error;
    }
    else
    {
        pDragging->ground_color = _context.gameConfig().display.ground_color_drag_ok;
        pDragging->border_color = _context.gameConfig().display.border_color_drag_ok;
    }

    pTrans->position = dstpos;
    pDragging->tip_pos = normalToGridPos(dstpos);

    return true;
}

bool GameScene::dropSelectActor(const Vec2& pos)
{
    if (!_context.registry().valid(_selectEntity))
    {
        return false;
    }

    auto pDragging = _context.registry().try_get<CompDragging>(_selectEntity);
    auto pTrans = _context.registry().try_get<CompTransform>(_selectEntity);
    if (!pDragging || !pTrans)
    {
        return false;
    }

    auto dstpos = camera().screenToWorld(pos);
    if (canDropToPos(dstpos))
    {
        dstpos = normalToGridPos(dstpos);
        _context.dispatcher().trigger(EvtRoleOnDrop{ _selectEntity, dstpos });
    }
    else
    {
        dstpos = pDragging->origin_pos;
    }

    pTrans->position = dstpos;

    addObjectToGrid(_selectEntity, getGridFromPos(dstpos));

    _context.registry().remove<CompDragging>(_selectEntity);

    auto bevComp = _context.registry().try_get<CompBehavior>(_selectEntity);
    if (bevComp && bevComp->bevtree)
    {
        bevComp->bevtree->start();
    }

    return true;
}

bool GameScene::canDropToPos(const Vec2& pos)
{
    auto walktype = getGridWalkType(getGridFromPos(pos));
    return walktype != (int)tilemap::WalkType::Collision;
}

} 