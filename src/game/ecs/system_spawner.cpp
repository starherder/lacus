#include "system_spawner.h"


namespace game 
{
    DeclareEcsSystem(SpawnerSystem, EcsPriority::Middle);

    SpawnerSystem::SpawnerSystem(GameContext& context) : EcsSystem(context)
    {
        _context.dispatcher().sink<EvtRoleDead>().connect<&SpawnerSystem::onRoleDead>(this);
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
                spawnActor(ent, spawner, trans);
            }
            else if(spawner.npc_set.size() < spawner.max_count)
            {
                if (spawner.cur_tick >= spawner.interval)
                {
                    if (spawner.npc_set.size() < spawner.max_count)
                    {
                        spawnActor(ent, spawner, trans);
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
                continue;
            }
        }
    }

    void SpawnerSystem::onRoleDead(const EvtRoleDead& e)
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

    void SpawnerSystem::spawnActor(entt::entity spawner, const CompSpawner& compSpawner, const CompTransform& compTrans)
    {
        std::optional<Vec2> optSpawnPos = compTrans.position;

        int LoopCount = 0;
        while (true)
        {
            if (LoopCount++ > 20)
            {
                LogWarn("SpawnerSystem::spawnActor: loop time > {}, break;", LoopCount);
                break;
            }

            float angle = utility::random(-3.14f, 3.14f);
            float dist = utility::random(compSpawner.radius / 2.0f, compSpawner.radius);

            Vec2 offset = Vec2{ sin(angle), cos(angle) } *dist;
            Vec2 dest = compTrans.position + offset;

            Vec2 srcGrid = _context.scene().getGridFromPos(compTrans.position);
            Vec2 dstGrid = _context.scene().getGridFromPos(dest);

            auto path = _context.pathFinder().findPath(srcGrid, dstGrid);
            if (!path)
            {
                continue;
            }

            auto walktypeOpt = _context.scene().getGridWalkType(dstGrid);
            if (walktypeOpt != (int)tilemap::WalkType::Collision)
            {
                optSpawnPos = dest;
                break;
            }
        }

        if (optSpawnPos)
        {
            auto npc = _context.scene().createObject(compSpawner.npc, optSpawnPos.value());
            if (_context.registry().valid(npc))
            {
                auto compComm = _context.registry().try_get<CompComm>(npc);
                if (compComm)
                {
                    compComm->side = compSpawner.side;
                }

                auto compSpawner = _context.registry().try_get<CompSpawner>(spawner);
                if (compSpawner)
                {
                    compSpawner->npc_set.insert(npc);
                }

                _context.registry().emplace<CompSpawnMe>(npc, CompSpawnMe{spawner});
            }
        }
    }
}