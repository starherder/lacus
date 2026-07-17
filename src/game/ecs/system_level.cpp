#include "system_level.h"

#include "game/logic/game_logic.h"
#include "game/ui/form_result.h"

namespace game
{
    DeclareEcsSystem(LevelSystem, EcsPriority::Middle);

    LevelSystem::LevelSystem(GameContext& context) : EcsSystem(context)
    {
        _context.dispatcher().sink<EvtRoleDead>().connect<&LevelSystem::onRoleDead>(this);
    }

    LevelSystem::~LevelSystem()
    {
    }

    void LevelSystem::onRoleDead(const EvtRoleDead& e)
    {
        if (e.event == "level_clear")
        {
            _context.gameLogic().finishGame(GameResult::Success);
        }
    }
}
