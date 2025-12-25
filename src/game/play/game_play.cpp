#include "game_play.h"
#include "components.h"
#include "engine/render.h"
#include "a_star/a_star.hpp"

namespace game {

GamePlay::GamePlay()
{
}

GamePlay::~GamePlay()
{
}

void GamePlay::setActorPos(entt::entity id, const Vec2& pos)
{
    if(!_registry.valid(id))
    {
        spdlog::warn("entity {} not exist.", (int32_t)id);
        return;
    }

    auto& transform = _registry.get<CompTransform>(id);
    transform.position = pos;
}

Vec2 GamePlay::getActorPos(entt::entity id)
{
    if(!_registry.valid(id))
    {
        spdlog::warn("entity {} not exist.", (int32_t)id);
        return {};
    }
    return _registry.get<CompTransform>(id).position;
}

void GamePlay::setMotionSpeed(entt::entity id, float speed)
{
    if(!_registry.valid(id))
    {
        spdlog::warn("entity {} not exist.", (int32_t)id);
        return;
    }
    auto& motion = _registry.get<CompMotion>(id);
    motion.speed = speed;
}

float GamePlay::getMotionSpeed(entt::entity id)
{
    if(!_registry.valid(id))
    {
        spdlog::warn("entity {} not exist.", (int32_t)id);
        return 0.0f;
    }
    return _registry.get<CompMotion>(id).speed;
}

void GamePlay::initPathFind(AStar::Generator* generator, const Vec2i& mapSize, const Vec2i& tileSize)
{
    _pathGenerator = generator;

    _mapSize = mapSize;
    _tileSize = tileSize;
}

void GamePlay::update(float deltaTime)
{
    motionSystem(deltaTime);
}

void GamePlay::draw(Renderer& renderer, const Camera& camera)
{
    renderSystem(renderer, camera);
}

entt::entity GamePlay::createActor(const std::string& name, const Vec2& pos, const Vec2& size)
{
    auto entid = _registry.create();
    _registry.emplace<CompNameId>(entid, entid, name);
    _registry.emplace<CompTransform>(entid, pos, size, Vec2{0.0f,0.0f}, Vec2{1.0f,1.0f});
    _registry.emplace<CompDisplay>(entid);
    _registry.emplace<CompState>(entid);
    _registry.emplace<CompMotion>(entid);

    _nameIdMap.insert({name, entid});
    return entid;
}

entt::entity GamePlay::getActor(const std::string& name)
{
    auto it = _nameIdMap.find(name);
    if(it != _nameIdMap.end())
    {
        return it->second;
    }

    return entt::null;
}

Vec2i GamePlay::getGridFromPos(const Vec2& pos)
{
    Vec2 gridf = pos / _tileSize;
    Vec2i grid = {(int)std::ceil(gridf.x) -1, (int)std::ceil(gridf.y) -1};
    return grid;
}

Vec2 GamePlay::getGridLeftTopPos(const Vec2i& grid)
{
    return {grid.x * _tileSize.x, grid.y * _tileSize.y};
}

Vec2 GamePlay::getGridCenterPos(const Vec2i& grid)
{
    return {(grid.x + 0.5f) * _tileSize.x, (grid.y+0.5f) * _tileSize.y};
}

bool GamePlay::reachGridCenter(const Vec2& pos, const Vec2i& grid, float epsilon)
{
    auto grid_pos = getGridCenterPos(grid);

    float dis =glm::distance(pos, grid_pos) ;
    spdlog::info("dis: {}, epsilon: {}", dis, epsilon);

    return dis < epsilon;
}

bool GamePlay::motionStart(entt::entity id, const Vec2& dst)
{
    if(!motionStop(id))
    {
        return false;
    }

    auto& transform = _registry.get<CompTransform>(id);
    auto& motion = _registry.get<CompMotion>(id);
    auto& src = transform.position;

    // from world pos to grid
    Vec2i srcGrid = getGridFromPos(src);
    Vec2i dstGrid = getGridFromPos(dst);

    spdlog::info("{} motion start: ({}, {}) -> ({}, {})", (int32_t)id, srcGrid.x, srcGrid.y, dstGrid.x, dstGrid.y);

    // path find
    auto path = _pathGenerator->findPath(srcGrid, dstGrid);
    if(path.empty())
    {
        spdlog::info("path find failed.");
        return false;
    }

    // add path
    motion.path.clear();
    for(auto& grid : path)
    {
        motion.path.push_back(grid);
    }

    // set state
    auto& state = _registry.get<CompState>(id);
    state.state = ActorState::Move;

    motion.velocity = glm::normalize(getGridCenterPos(motion.path.back()) - src) * motion.speed;
    motion.running = true;
    return true;
}

bool GamePlay::motionStop(entt::entity id)
{
    if(!_registry.valid(id))
    {
        spdlog::warn("entity {} not exist.", (int32_t)id);
        return false;
    }

    spdlog::info("motion stop: {} ");

    auto& motion = _registry.get<CompMotion>(id);
    motion.path.clear();
    motion.velocity = {0, 0};
    motion.running = false;

    auto& state = _registry.get<CompState>(id);
    state.state = ActorState::Idle;
    return true;
}

bool GamePlay::motionPause(entt::entity id, bool pause)
{
    if(!_registry.valid(id))
    {
        spdlog::warn("entity {} not exist.", (int32_t)id);
        return false;
    }
    
    auto& motion = _registry.get<CompMotion>(id);
    motion.running = !pause;
    return true;
}

ActorState GamePlay::getActorState(entt::entity id)
{
    return _registry.get<CompState>(id).state;
}

void GamePlay::destroyActor(entt::entity id)
{
    if(!_registry.valid(id))
    {
        spdlog::warn("entity {} not exist.", (int32_t)id);
        return;
    }
    _registry.destroy(id);
}

void GamePlay::destroyActor(const std::string& name)
{
    auto id = getActor(name);
    if(id != entt::null)
    {
        destroyActor(id);
    }
}

void GamePlay::destroyAllActor()
{
    _registry.clear();
}

void GamePlay::motionSystem(float deltaTime)
{
    auto ent_view = _registry.view<CompTransform, CompMotion, CompState>();
    for(auto& ent : ent_view)
    {
        auto& transform = ent_view.get<CompTransform>(ent);
        auto& motion = ent_view.get<CompMotion>(ent);
        auto& state = ent_view.get<CompState>(ent);
        auto& pos = transform.position;

        if(state.state == ActorState::Move)
        {
            if(!motion.running)
            {
                continue;
            }

            if(motion.path.empty())
            {
                if(glm::distance(motion.targetPos, pos) <= motion.speed*deltaTime)
                {
                    spdlog::info("{} motion finish: at ({}, {})", (int32_t)ent, motion.targetPos.x, motion.targetPos.y);

                    motionStop(ent);
                    setActorPos(ent, motion.targetPos);
                }
                continue;
            }

            const auto& cur_grid = getGridFromPos(pos);
            const auto& next_grid = motion.path.back();

            if(next_grid != cur_grid )
            {
                motion.velocity = glm::normalize(getGridCenterPos(next_grid) - pos) * motion.speed;
            }
            else 
            {
                if(reachGridCenter(pos, next_grid, motion.speed*deltaTime))
                {
                    spdlog::info("{} motion: reach ({}, {}), go next step ({}, {})", 
                        (int32_t)ent, cur_grid.x, cur_grid.y, next_grid.x, next_grid.y);

                    motion.path.pop_back();
                }
            }

            transform.position += motion.velocity * deltaTime;
        }
    }
}

void GamePlay::renderSystem(Renderer& renderer, const Camera& camera)
{
    auto ent_view = _registry.view<CompNameId, CompTransform, CompDisplay>();
    for(auto& ent : ent_view)
    {
        auto& nameid = ent_view.get<CompNameId>(ent);
        auto& transform = ent_view.get<CompTransform>(ent);
        auto& display = ent_view.get<CompDisplay>(ent);

        auto dstrect = Rect{ transform.position-transform.size/2.0f, transform.size };
        dstrect = camera.projectRect(dstrect);

        if(display.texture != nullptr)
        {
            auto srcrect = display.texture_rect;
            renderer.drawTexture(display.texture, srcrect, dstrect);
        }
        else 
        {
            renderer.setDrawColor(display.ground_color);
            renderer.drawFillRect(dstrect);

            renderer.setDrawColor(display.border_color);
            renderer.drawRect(dstrect);

            renderer.drawDebugText(dstrect.pos()+Vec2{10,10}, nameid.name.c_str());
        }
    }

    if(_debugMode)
    {
       drawMotionDebug(renderer, camera);
    }
}

void GamePlay::drawMotionDebug(Renderer& renderer, const Camera& camera)
{
    auto ent_view = _registry.view<CompTransform, CompMotion, CompState>();
    for(auto& ent : ent_view)
    {
        auto& transform = ent_view.get<CompTransform>(ent);
        auto& motion = ent_view.get<CompMotion>(ent);
        auto& state = ent_view.get<CompState>(ent);

        if(state.state == ActorState::Move && motion.path.size() > 0 )
        {
            renderer.setDrawColor(Color::Red);
            auto lstPos = camera.projectPoint(transform.position);

            for(auto it=motion.path.rbegin(); it!=motion.path.rend(); ++it)
            {
                auto grid_center = getGridCenterPos(*it);
                auto grid_pos = camera.projectPoint(grid_center);

                renderer.drawLine(lstPos, grid_pos);

                auto rect = Rect{grid_center-Vec2{10,10}, Vec2{20,20}};
                rect = camera.projectRect(rect);

                renderer.drawRect(rect);
                lstPos = grid_pos;
            }
        }
    }  
}






}