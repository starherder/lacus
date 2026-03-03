#include "game_scene.h"
#include "game/game_config.h"

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

    _context.dispatcher().sink<EvtObjectSelection>().connect<&GameScene::onRoleSelect>(this);
    _context.dispatcher().sink<EvtObjectUnselect>().connect<&GameScene::onRoleUnselect>(this);
    
    _context.dispatcher().sink<EvtMotionStop>().connect<&GameScene::onRoleMotionStop>(this);

    _context.eventDispatcher().onMouseLeftDown.connect(this, &GameScene::onMouseLeftPressed, -1);
    _context.eventDispatcher().onMouseLeftDrag.connect(this, &GameScene::onMouseLeftDrag, -1);

    _context.eventDispatcher().onMouseLeftDragStart.connect(this, &GameScene::onMouseLeftDragStart, -1);
    _context.eventDispatcher().onMouseLeftDragFinish.connect(this, &GameScene::onMouseLeftDragFinish, -1);

    _context.eventDispatcher().onMouseLeftClicked.connect(this, &GameScene::onMouseLeftClick, -1);
    _context.eventDispatcher().onMouseRightClicked.connect(this, &GameScene::onMouseRightClick, -1);

    _context.eventDispatcher().onKeyDown.connect(this, &GameScene::onKeyDown, -1);

    _context.eventDispatcher().onMouseMotion.connect(this, &GameScene::onMouseMotion, -1);
}

GameScene::~GameScene()
{
}

Vec2 GameScene::sceneSize()
{
    return mapSize() * tileSize();
}

Vec2 GameScene::scenePos()
{
    return {0, 0};
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

    std::thread loadthread([this, mapPath]() 
    {
        loadInThread(mapPath);
    });

    loadthread.detach();
    return true;
}

bool GameScene::unload()
{
    _state = (int)SceneState::Unloading;

    std::thread unloadthread([this]() 
    {
        unloadInThread();
    });

    unloadthread.detach();
    return true;
}

void GameScene::loadInThread(const engine::fs::path& mapPath)
{
    int loop_count = 0;
    while (_state != (int)SceneState::None)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        if (loop_count++ > 20000)
        {
            LogError("LOOP wait tooo long. exit");
            return;
        }
    }

    _state = (int)SceneState::Loading;

    LogInfo("load in thread start:  map={}", mapPath.string());

    on_load_progress.emit(0.0f);

    // TODO: remove this
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    on_load_progress(0.2f);

    auto res = _tileMap.load(mapPath);
    if(!res)
    {
        LogError("load map ({}) failed.", mapPath.string());
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

    initQuadTree();

    // TODO: remove this
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    on_load_progress(0.9f);

    loadObjects();

    // TODO: remove this
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    on_load_progress(1.0f);

    _state = (int)SceneState::Ready;
    LogInfo("load scene (map={}) finished.", mapPath.string());
}

void GameScene::unloadInThread()
{
    LogInfo("unload in thread.");

    auto res = _tileMap.unload();
    if(!res)
    {
        LogError("tile map unload failed.");
        return ;
    }

    unloadObjects();

    _state = (int)SceneState::None;
    LogInfo("unload scene OK.");
}

void GameScene::onUpdate(float deltaTime)
{
    _camera.update(deltaTime);

    _quadtree->clear();
    for (auto& obj : _sceneObjects)
    {
        _quadtree->add(obj);
    }
}

void GameScene::onDraw() 
{
    auto& renderer = application().renderer();

    _tileMap.draw(renderer, _camera);
}

void GameScene::onStart()
{
    LogInfo("========================= GameScene::onStart =========================");
}

void GameScene::onStop()
{
    LogInfo("========================= GameScene::onStop =========================");
}

void GameScene::setDebugInfo(bool show)
{
    _context.setDebugMode(show);
}

void GameScene::initQuadTree()
{
    auto sceneSize = (_tileMap.mapSize() + Vec2i{1,1})*_tileMap.tileSize();
    auto scenebox = BoxType(0.0f, 0.0f, sceneSize.x, sceneSize.y);

    auto getAABB = [this](const entt::entity ent)
    {
        auto comp = _context.registry().try_get<CompTransform>(ent);
        if (comp)
        {
            auto& pos = comp->position;
            auto& sz = comp->size;
            return BoxType{ pos.x, pos.y, sz.x, sz.y };
        }

        return BoxType{0, 0, 0, 0};
    };

    _quadtree = std::make_unique<QuadTreeType>(scenebox, getAABB);
}

void GameScene::initPathFind()
{
    // 通用寻路
    _context.pathFinder().clear();
    _context.pathFinder().setWorldSize(_tileMap.mapSize());
    _context.pathFinder().setHeuristic(AStar::Heuristic::euclidean);
    _context.pathFinder().setDiagonalMovement(false);

    // 陆地寻路
    _context.landPathFinder().clear();
    _context.landPathFinder().setWorldSize(_tileMap.mapSize());
    _context.landPathFinder().setHeuristic(AStar::Heuristic::euclidean);
    _context.landPathFinder().setDiagonalMovement(false);

    // 障碍物
    for(auto& grid : _tileMap.collisionPoints()) 
    {
        _context.pathFinder().addCollision(grid);
        _context.landPathFinder().addCollision(grid);
    }

    // 陆地寻路，水面也是障碍物
    for (auto& grid : _tileMap.waterPoints())
    {
        _context.landPathFinder().addCollision(grid);
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
    if (layer) 
    {
        for (auto& [id, obj] : layer->objects) 
        {
            auto optSide = _tileMap.getObjectProperty<int>(obj.id, "side");
            auto side = optSide ? (CampSide)optSide.value() : CampSide::None;

            createObjectInScene(obj.name, obj.pos, side);
        }
    }
}

void GameScene::unloadObjects()
{
    _context.registry().clear();

    _collisionDebugRects.clear();
}

entt::entity GameScene::findObjectAtPos(const Vec2& pos)
{
    auto grid = getGridFromPos(pos);
    const auto& objset = getObjectsInGrid(grid);
    if (objset.empty()) 
    {
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
                return obj;
            }
        }
    }

    return entt::null;
}

entt::entity GameScene::createObjectInScene(const std::string& cfgid, const Vec2& pos, CampSide side)
{
    auto ent = ObjectFactory::inst().createObject(cfgid);
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

    auto pcomm = _context.registry().try_get<CompComm>(ent);
    if (pcomm)
    {
        pcomm->side = side;

        if (pcomm->side == CampSide::Gangster)
        {
            _context.registry().emplace<CompAutoPick>(ent);
        }
    }

    _sceneObjects.insert(ent);
    return ent;
}

void GameScene::destroyObject(entt::entity id)
{
    if (!_registry.valid(id))
    {
        LogWarn("entity {} not exist.", (int32_t)id);
        return;
    }

    _registry.emplace_or_replace<CompDestroy>(id);
}


void GameScene::addObjectToScene(entt::entity id)
{
    _sceneObjects.insert(id);
}

void GameScene::removeObjectFromScene(entt::entity id)
{
    _sceneObjects.erase(id);
}

void GameScene::onRoleDestroyed(const EvtRoleDestroyed& e)
{
    _sceneObjects.erase(e.actor);
}

Vec2i GameScene::getObjectGrid(entt::entity id)
{
    auto comp = _registry.try_get<CompTransform>(id);
    assert(comp && "CompTransform NOT found.");

    return getGridFromPos(comp->position);
}

Vec2 GameScene::getObjectPos(entt::entity id)
{
    auto comp = _registry.try_get<CompTransform>(id);
    assert(comp && "CompTransform NOT found.");

    return comp->position;
}

void GameScene::setObjectPos(entt::entity id, const Vec2& pos)
{
    auto comp = _registry.try_get<CompTransform>(id);
    if (comp)
    {
        comp->position = pos;
    }
}

void GameScene::onRoleCrossGrid(const EvtRoleCrossGrid& e)
{
}

void GameScene::onRoleSelect(const EvtObjectSelection& e)
{
    auto bevComp = _context.registry().try_get<CompBehavior>(e.object);
    if (bevComp && bevComp->bevtree)
    {
        bevComp->bevtree->stop();
    }

    on_select_object(e.object);

}
void GameScene::onRoleUnselect(const EvtObjectUnselect& e)
{
    auto bevComp = _context.registry().try_get<CompBehavior>(e.object);
    if (bevComp && bevComp->bevtree)
    {
        bevComp->bevtree->start();
    }

    on_unselect_object(e.object);
}

void GameScene::onRoleMotionStop(const EvtMotionStop& e)
{
}

GameScene::EntityVector GameScene::getObjectsInGrid(const Vec2i& grid)
{ 
    auto pos = getGridLeftTopPos(grid);
    auto sz = _tileMap.tileSize();
    
    BoxType box = {pos.x, pos.y, (float)sz.x, (float)sz.y};

    if (_quadtree)
    {
        return _quadtree->query(box);
    }

    return EntityVector{};
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

const GameScene::EntityDisMap& GameScene::getObjectsInCircle(const Vec2& center, float radius)
{
    static std::multimap<float, entt::entity> result;
    result.clear();

    if (radius == 0)
    {
        auto grid = getGridFromPos(center);
        auto objects = getObjectsInGrid(grid);
        for (auto& obj : objects)
        {
            result.insert({ 0, obj });
        }
        return result;
    }

    auto objects = _quadtree->query(quadtree::Circle<float>{ quadtree::Vector2{ center.x, center.y }, radius });
    for (auto& obj : objects)
    {
        auto trans = _registry.try_get<CompTransform>(obj);
        if (trans)
        {
            float dis = glm::distance(trans->position, center);
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
    auto selobj = findObjectAtPos(scenePos);
    if (selobj != _selectEntity && _selectEntity != entt::null)
    {
        _context.dispatcher().trigger(EvtObjectUnselect{ _selectEntity });
    }

    _selectEntity = selobj;

    _context.dispatcher().trigger(EvtRoleStopMotion{ _selectEntity });
    _context.dispatcher().trigger(EvtObjectSelection{ _selectEntity });
}

void GameScene::onMouseLeftRelease(const Vec2& pos)
{
}

void GameScene::onMouseLeftDrag(const Vec2& pos, const Vec2& offset)
{
    if (objectDragable() && dragSelectActorInProgress(pos))
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
    if (!_context.registry().valid(_selectEntity)) {
        return;
    }

    auto pcomm = _context.registry().try_get<CompComm>(_selectEntity);
    if (!pcomm || pcomm->side != CampSide::Gangster) {
        return;
    }

    auto scenePos = camera().screenToWorld(pos);
    moveSelectActor(scenePos);
}

void GameScene::onMouseMotion(const Vec2& pos, const Vec2& offset)
{
    auto scenePos = camera().screenToWorld(pos);
    auto hoverEntity = findObjectAtPos(scenePos);
    if (hoverEntity == _hoverEntity)
    {
        return;
    }

    if (hoverEntity == entt::null) 
    {
        onLeaveObject(_hoverEntity);
    }
    else
    {
        onHoverObject(hoverEntity);
    }

    _hoverEntity = hoverEntity;
}

void GameScene::onKeyDown(KeyCode key)
{
    switch (key)
    {
    case SDLK_W: return onMoveStep({0, -1});
    case SDLK_S: return onMoveStep({0, 1});
    case SDLK_A: return onMoveStep({-1, 0});
    case SDLK_D: return onMoveStep({1, 0});
    case SDLK_SPACE: return onSkipMove();
    default: return;
    }
}

void GameScene::onMoveStep(const Vec2i& dir)
{
    if (!_context.registry().valid(_selectEntity)) 
    { 
        return; 
    }

    if (!_context.registry().try_get<CompMoveCfg>(_selectEntity))
    {
        return;
    }

    uint8_t moveGrids = 1;
    _context.dispatcher().trigger(EvtStepMove{ _selectEntity, dir, moveGrids });
}

void GameScene::onSkipMove()
{
    _context.dispatcher().trigger(EvtGameTurnFinish{_selectEntity, GameTurnType::Moving});
}

void GameScene::onHoverObject(entt::entity obj)
{
    LogInfo("hover object: {}", obj);
    on_hover_object.emit(obj);
}

void GameScene::onLeaveObject(entt::entity obj)
{
    LogInfo("leave object: {}", obj);
    on_leave_object.emit(obj);
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

    if (_context.registry().try_get<CompMoveCfg>(_selectEntity))
    {
        auto gridPos = getGridFromPos(pos);
        _context.dispatcher().trigger(EvtMoveToGrid{ _selectEntity, gridPos, true });
    }
}

bool GameScene::dragSelectActor(const Vec2& pos)
{
    if (!_context.registry().valid(_selectEntity) || !objectDragable())
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

    auto bevComp = _context.registry().try_get<CompBehavior>(_selectEntity);
    if (bevComp && bevComp->bevtree)
    {
        bevComp->bevtree->stop();
    }

    removeObjectFromScene(_selectEntity);

    return true;
}

bool GameScene::dragSelectActorInProgress(const Vec2& pos)
{
    if (!_context.registry().valid(_selectEntity) || !objectDragable())
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
    if (!_context.registry().valid(_selectEntity) || !objectDragable())
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

    addObjectToScene(_selectEntity);

    setObjectPos(_selectEntity, dstpos);

    _context.registry().remove<CompDragging>(_selectEntity);

    auto bevComp = _context.registry().try_get<CompBehavior>(_selectEntity);
    if (bevComp && bevComp->bevtree)
    {
        bevComp->bevtree->start();
    }

    EvtRoleCrossGrid e;
    e.actor = _selectEntity;
    e.cur_grid = getObjectGrid(_selectEntity);
    e.lst_grid = getObjectGrid(_selectEntity);
    _context.dispatcher().trigger(e);

    return true;
}

bool GameScene::canDropToPos(const Vec2& pos)
{
    auto walktype = getGridWalkType(getGridFromPos(pos));
    return walktype != (int)tilemap::WalkType::Collision;
}

} 