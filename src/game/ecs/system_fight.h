
#include "comm_system.h"
#include "comm_comp.h"
#include "comm_event.h"
#include "game/scene/game_camera.h"
#include "game/scene/game_scene.h"


namespace game
{

    class FightSystem : public EcsSystem
    {
        enum class FuncOperate {
            Plus,
            Minus,
            Unknow,
        };

        enum class FuncUnitType {
            Value,
            Multi,
            Percent,
            Unknow,
        };

        struct FuncFactor
        {
            std::string key;

            FuncOperate operate = FuncOperate::Unknow;

            float fval = 0.f;
            std::string sval;

            FuncUnitType unit = FuncUnitType::Unknow;
        };

        using FuncFactorOpt = std::optional<std::vector<FuncFactor>>;

    public:
        FightSystem(GameContext& context);
        ~FightSystem();

        void update(float delta) override;

        void onRoleUnderAttack(const RoleOnAttack& e);

        void onRolePropAlter(const RolePropAlter& e);
        void onRoleLevelAlter(const RoleLevelAlter& e);


        void applyFuncToTarget(FuncFactor fac, entt::entity source, entt::entity target);

        void addBuf(entt::entity target, const std::string& buf);

        void removeBuf(entt::entity target, const std::string& buf);

        void addHpToTarget(entt::entity target, float hp);

        void showHpFloatingTip(entt::entity target, float hp);

        FuncFactorOpt parseFightFunc(const std::string& str);
    };



}