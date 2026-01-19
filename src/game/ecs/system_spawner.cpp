#include "system_spawner.h"


namespace game 
{

    SpawnerSystem::SpawnerSystem(GameContext& context) : EcsSystem(context)
    {
        _context.dispatcher().sink<RoleDead>().connect<&SpawnerSystem::onRoleDead>(this);
    }

    SpawnerSystem::~SpawnerSystem()
    {
    }

    void SpawnerSystem::update(float deltaTime)
    {
        auto views = _context.registry().view<CompTransform, CompSpawner>();
        for(auto& ent : views)
        {
            auto& trans = views.get<CompTransform>(ent);
            auto& spawner = views.get<CompSpawner>(ent);
            if(spawner.npc_set.size() < spawner.min_count)
            {
                spawnActor(ent, spawner.npc, trans.position, spawner.radius);
            }
            else if(spawner.npc_set.size() < spawner.max_count)
            {
                if (spawner.cur_tick >= spawner.interval)
                {
                    if (spawner.npc_set.size() < spawner.max_count)
                    {
                        spawnActor(ent, spawner.npc, trans.position, spawner.radius);
                    }
                    spawner.cur_tick = 0;
                }
                else
                {
                    spawner.cur_tick += _context.applicaton().frameTicker().deltaTicks();
                }
            }
            else
            {
                return;
            }
        }
    }

    void SpawnerSystem::onRoleDead(const RoleDead& e)
    {
        auto pCompSpawner = _context.registry().try_get<CompSpawnMe>(e.actor);
        if(pCompSpawner)
        {
            auto entSpawner = pCompSpawner->who_spawn_me;
            auto pSpawn = _context.registry().try_get<CompSpawner>(entSpawner);
            if(pSpawn)
            {
                pSpawn->npc_set.erase(e.actor);
            }

        }
    }

    void SpawnerSystem::spawnActor(entt::entity spawner, const std::string& cfgid, const Vec2& pos, float radius)
    {
        std::optional<Vec2> optSpawnPos = pos;

        int LoopCount = 0;
        while (true)
        {
            if (LoopCount++ > 20)
            {
                spdlog::warn("SpawnerSystem::spawnActor: loop time > {}, break;", LoopCount);
                break;
            }

            float angle = utility::random(-3.14f, 3.14f);
            float dist = utility::random(radius / 2.0f, radius);

            Vec2 offset = Vec2{ sin(angle), cos(angle) } *dist;
            Vec2 dest = pos + offset;

            Vec2 srcGrid = _context.currentScene().getGridFromPos(pos);
            Vec2 dstGrid = _context.currentScene().getGridFromPos(dest);

            auto path = _context.pathFinder().findPath(srcGrid, dstGrid);
            if (!path)
            {
                continue;
            }

            auto walktypeOpt = _context.currentScene().mapInfo().getTileProperty<int>(dstGrid.x, dstGrid.y, "walktype");
            if (walktypeOpt && walktypeOpt.value() != (int)tilemap::WalkType::Collision)
            {
                optSpawnPos = dest;
                break;
            }
        }

        if (optSpawnPos)
        {
            auto npc = _context.currentScene().createActor(cfgid, optSpawnPos.value());
            if (_context.registry().valid(npc))
            {
                _context.registry().emplace<CompSpawnMe>(npc, CompSpawnMe{ spawner });

                auto compSpawner = _context.registry().try_get<CompSpawner>(spawner);
                if (compSpawner)
                {
                    compSpawner->npc_set.insert(npc);
                }
            }
        }
    }
}