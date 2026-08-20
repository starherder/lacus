#include "game_scene.h"
#include "game/game_config.h"
#include "game/scene/scene_config.h"
#include "game/logic/game_play_tile_battle.h"
#include "game/logic/game_play_auto_chess.h"

#ifdef _DevelopMode
#include "game/ui/imform_debug.h"
#endif

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
    _context.dispatcher().sink<EvtMotionStop>().connect<&GameScene::onRoleMotionStop>(this);

    _context.eventDispatcher().onMouseMotion.connect(this, &GameScene::onMouseMotion, -1);

    // 默认玩法
    _gamePlay = std::make_unique<GamePlayTileBattle>(_context);
    _context.setGamePlay(_gamePlay.get());
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

Vec2i GameScene::getObjectGridSize(const CompTransform& trans)
{
    auto tile = tileSize();
    auto size = trans.base_size.length() > 1e-8f ? trans.base_size : trans.size;
    return {
        std::max(1, (int)std::ceil(size.x / tile.x)),
        std::max(1, (int)std::ceil(size.y / tile.y))
    };
}

Vec2i GameScene::getObjectLeftTopGrid(const CompTransform& trans)
{
    auto grid = getGridFromPos(trans.position);
    auto gridSize = getObjectGridSize(trans);
    return {
        grid.x - (gridSize.x % 2 == 0 ? 0 : gridSize.x / 2),
        grid.y - (gridSize.y % 2 == 0 ? 0 : gridSize.y / 2)
    };
}

std::vector<Vec2i> GameScene::getObjectGrids(const CompTransform& trans)
{
    auto leftTopGrid = getObjectLeftTopGrid(trans);
    auto gridSize = getObjectGridSize(trans);

    std::vector<Vec2i> grids;
    grids.reserve(gridSize.x * gridSize.y);
    for (int y = 0; y < gridSize.y; ++y)
    {
        for (int x = 0; x < gridSize.x; ++x)
        {
            grids.push_back({ leftTopGrid.x + x, leftTopGrid.y + y });
        }
    }

    return grids;
}

std::vector<Vec2i> GameScene::getObjectMoveEnterGrids(const CompTransform& trans, const Vec2i& dir)
{
    auto leftTopGrid = getObjectLeftTopGrid(trans);
    auto gridSize = getObjectGridSize(trans);

    std::vector<Vec2i> grids;
    if (dir.x > 0)
    {
        grids.reserve(gridSize.y);
        int x = leftTopGrid.x + gridSize.x;
        for (int y = 0; y < gridSize.y; ++y)
        {
            grids.push_back({ x, leftTopGrid.y + y });
        }
    }
    else if (dir.x < 0)
    {
        grids.reserve(gridSize.y);
        int x = leftTopGrid.x - 1;
        for (int y = 0; y < gridSize.y; ++y)
        {
            grids.push_back({ x, leftTopGrid.y + y });
        }
    }
    else if (dir.y > 0)
    {
        grids.reserve(gridSize.x);
        int y = leftTopGrid.y + gridSize.y;
        for (int x = 0; x < gridSize.x; ++x)
        {
            grids.push_back({ leftTopGrid.x + x, y });
        }
    }
    else if (dir.y < 0)
    {
        grids.reserve(gridSize.x);
        int y = leftTopGrid.y - 1;
        for (int x = 0; x < gridSize.x; ++x)
        {
            grids.push_back({ leftTopGrid.x + x, y });
        }
    }

    return grids;
}

Rect GameScene::getGridAlignedAABB(const CompTransform& trans)
{
    auto tile = tileSize();
    auto leftTopGrid = getObjectLeftTopGrid(trans);
    auto gridSize = getObjectGridSize(trans);
    auto pos = getGridLeftTopPos(leftTopGrid);
    Vec2 sz = { gridSize.x * tile.x, gridSize.y * tile.y };

    return Rect{ pos, sz };
}

Rect GameScene::getDisplayAABB(const CompTransform& trans)
{
    auto tile = tileSize();
    auto gridSize = getObjectGridSize(trans);
    Vec2 offset = {
        gridSize.x % 2 == 0 ? (gridSize.x / 2.0f - 0.5f) * tile.x : 0.0f,
        gridSize.y % 2 == 0 ? (gridSize.y / 2.0f - 0.5f) * tile.y : 0.0f
    };

    return Rect{ trans.position + trans.visual_offset - trans.size / 2.0f + offset, trans.size };
}

Vec2 GameScene::normalToGridPos(const Vec2& pos)
{
    return getGridCenterPos(getGridFromPos(pos));
}

void GameScene::normalToGridPos(entt::entity ent)
{
    if(!_context.registry().valid(ent))
    {
        return;
    }

    auto pTrans = _context.registry().try_get<CompTransform>(ent);
    if(!pTrans)
    {
        return;
    }

    auto pos = normalToGridPos(pTrans->position);
    pTrans->position = pos;
}

bool GameScene::load(const std::string& id)
{
    _camera.setPos({ 0, 0 });

    auto mapCfg = _context.sceneConfig().getMapConfig(id);
    if (!mapCfg)
    {
        LogError("load scene ({}) failed, not found.", id);
        return false;
    }

    std::thread loadthread([this, mapCfg]() 
    {
        loadInThread(*mapCfg);
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

void GameScene::loadInThread(const MapConfig& mapCfg)
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

    LogInfo("load in thread start: map={}", mapCfg.map_path);

    on_load_progress.emit(0.0f);

    if (mapCfg.play_type == GamePlayType::GamePlay_TileBattle)
    {
        _gamePlay = std::make_unique<GamePlayTileBattle>(_context); 
    }
    else if (mapCfg.play_type == GamePlayType::GamePlay_AutoChess)
    {
        _gamePlay = std::make_unique<GamePlayAutoChess>(_context);    
    }
    
    _context.setGamePlay(_gamePlay.get());

    // TODO: remove this
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    on_load_progress(0.2f);

    auto mapPath = _context.resPath() / mapCfg.map_path;
    
    auto res = _tileMap.load(mapPath.string());
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

    _gamePlay->update(deltaTime);

#ifdef _DevelopMode
    auto form = imgui::ImFormManager::inst().getForm<ImFormDebug>("ImFormDebug");
    auto pBotPlay = dynamic_cast<GamePlayTileBattle*>(_gamePlay.get());
    if (form && pBotPlay)
    {
        form->showTurnInfo((int)pBotPlay->getTurn());
    }
#endif
}

void GameScene::onDraw() 
{
    auto& renderer = application().renderer();

    _tileMap.draw(renderer, _camera);

    _gamePlay->draw();

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
    auto sceneSize = _tileMap.mapSize() * _tileMap.tileSize();
    auto scenebox = BoxType(0.0f, 0.0f, sceneSize.x, sceneSize.y);

    auto getAABB = [this](const entt::entity ent)
    {
        auto comp = _context.registry().try_get<CompTransform>(ent);
        if (comp)
        {
            auto rect = getGridAlignedAABB(*comp);
            return BoxType{ rect.x, rect.y, rect.w, rect.h };
        }

        return BoxType{0, 0, 0, 0};
    };

    _quadtree = std::make_unique<QuadTreeType>(scenebox, getAABB);
    //_quadtree->setQueryMode(quadtree::QueryMode::ContainCenter);
    _quadtree->setQueryMode(quadtree::QueryMode::Intersect);
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
    auto ent = ObjectManager::inst().createObject(cfgid);
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

    _context.gamePlay().onActorCreate(ent);

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

    _context.gamePlay().onActorDestroy(id);
    
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

void GameScene::onRoleMotionStop(const EvtMotionStop& e)
{
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

GameScene::EntitySet GameScene::getObjectsInGrid(const Vec2i& grid)
{
    auto pos = getGridLeftTopPos(grid);
    auto sz = _tileMap.tileSize();
    BoxType box = {pos.x, pos.y, (float)sz.x, (float)sz.y};

    EntitySet result;
    
    if (_quadtree)
    {
        auto queryResult = _quadtree->query(box);
        std::copy(queryResult.begin(), queryResult.end(), std::inserter(result, result.begin()));
    }

    return result;
}
    
entt::entity GameScene::getOneObjectInGrid(const Vec2i& grid, ObjectType type)
{
    auto objects = getObjectsInGrid(grid);
    if (objects.empty())
    {
        return entt::null;
    }
    
    for (auto& obj : objects)
    {
        auto typeComp = _registry.try_get<CompComm>(obj);
        if (typeComp && typeComp->type == type)
        {
            return obj;
        }
    }
    
    return entt::null;
}
    
bool GameScene::hasObjectInGrid(const Vec2i& grid, ObjectType type)
{
    auto objects = getObjectsInGrid(grid);
    if (type == ObjectType::All)
    {
        return !objects.empty();
    }
    
    for (auto& obj : objects)
    {
        auto typeComp = _registry.try_get<CompComm>(obj);
        if (typeComp && typeComp->type == type)
        {
            return true;
        }
    }
    return false;
}
    
GameScene::EntitySet GameScene::getObjectsInRect(const Rect& rect)
{
    BoxType box = { rect.x, rect.y, rect.w, rect.h };

    box.left = box.width < 0 ? box.left + box.width : box.left;
    box.top = box.height < 0 ? box.top + box.height : box.top;
    box.width = std::fabs(box.width);
    box.height = std::fabs(box.height);

    EntitySet result;
    if (_quadtree)
    {
        auto qureyResult = _quadtree->query(box);
        std::copy(qureyResult.begin(), qureyResult.end(), std::inserter(result, result.begin()));
    }
    return result;
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

void GameScene::onMouseMotion(const Vec2& pos, const Vec2& offset)
{
    auto scenePos = camera().screenToWorld(pos);
    
#ifdef _DevelopMode
    auto grid = _context.scene().getGridFromPos(scenePos);
    auto objs = _context.scene().getObjectsInGrid(grid);
    auto form = imgui::ImFormManager::inst().getForm<ImFormDebug>("ImFormDebug");
    if (form)
    {
        form->showGridInfo(grid, objs);
    }
#endif
    
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

void GameScene::onHoverObject(entt::entity obj)
{
    //LogInfo("hover object: {}", obj);
    on_hover_object.emit(obj);
}

void GameScene::onLeaveObject(entt::entity obj)
{
    //LogInfo("leave object: {}", obj);
    on_leave_object.emit(obj);
}

void GameScene::drawQuadTree()
{
    drawQuadNode(_quadtree->getRoot());
}
        
void GameScene::drawQuadNode(QuadTreeType::Node* node)
{
    if (!node) return;

    Vec2 pos = camera().projectPoint({node->box.left, node->box.top});
    Vec2 size = { node->box.width, node->box.height };

    _context.painter().drawRect(Color::White, {pos, size}, 0.0f, 2.0f);

    for (auto& child : node->children)
    {
        drawQuadNode(child.get());
    }
}


} 