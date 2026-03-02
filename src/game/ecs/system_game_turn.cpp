#include "system_game_turn.h"


namespace game {

    DeclareEcsSystem(GameTurnSystem, EcsPriority::Middle);

    GameTurnSystem::GameTurnSystem(GameContext& context) : EcsSystem(context)
    {

    }

    GameTurnSystem::~GameTurnSystem()
    {

    }

    void GameTurnSystem::update(float deltaTime)
    {

    }


}