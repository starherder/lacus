#pragma once


#include "comm_system.h"
#include "comm_comp.h"
#include "comm_event.h"
#include "game/scene/game_camera.h"
#include "game/scene/game_scene.h"
#include "game/scene/game_context.h"

#include "utility/i_singleton.h"

namespace game
{

	class SystemUtils 
	{
    public:
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

		static FuncFactorOpt parseFightFunc(const std::string& str);
	};

    using EcsSystemPtr = std::shared_ptr<EcsSystem>;


    class EcsSystemCreator
    {
    public:
        virtual EcsPriority priority() = 0;
        virtual EcsSystemPtr create(GameContext& context) = 0;
    };

    using EcsSystemCreatorPtr = std::shared_ptr<EcsSystemCreator>;



    class EcsSystemManager : public utility::ISingleton<EcsSystemManager>
    {
    public:
        using EcsSystemMap = std::multimap<EcsPriority, EcsSystemPtr>;

    public:
        void init(GameContext& context);

        void update(float delta);

        void draw();

        template<typename CreatorType>
        void addCreator();

    private:
        GameContext* _context = nullptr;
        EcsSystemMap _systems;
        std::vector<EcsSystemCreatorPtr> _creators;
    };

    template<typename CreatorType>
    void EcsSystemManager::addCreator()
    {
        auto creator = std::make_shared<CreatorType>();
        _creators.push_back(creator);
    }


#define DeclareEcsSystem(SystemType, Prio)                  \
    struct SystemType##_Creator : public EcsSystemCreator { \
        EcsPriority priority() override { return Prio; }    \
        EcsSystemPtr create(GameContext& context) override  \
        { return std::make_shared<SystemType>(context); }   \
    };                                                      \
    struct SystemType##_Initializer{                        \
        SystemType##_Initializer() {                        \
            EcsSystemManager::inst()                        \
                .addCreator<SystemType##_Creator>();        \
        }                                                   \
        ~##SystemType##_Initializer() {}                    \
    }g_##SystemType##_Initializer;
    


}