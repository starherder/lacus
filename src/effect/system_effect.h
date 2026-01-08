#include "comp_effect.h"
#include "game/ecs/comm_system.h"

namespace game {


    class EffectSystem : public EcsSystem
    {
    public:
        EffectSystem(GameContext& context) : EcsSystem(context) {}
        ~EffectSystem() {}

        void update(float delta) override;

    };

}