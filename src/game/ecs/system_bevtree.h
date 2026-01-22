#pragma once


#include "system_utils.h"


namespace game {

class BevTreeSystem : public EcsSystem
{
public:
    BevTreeSystem(GameContext& context) : EcsSystem(context) {}
    ~BevTreeSystem() {}

    void update(float delta) override;
};

}

