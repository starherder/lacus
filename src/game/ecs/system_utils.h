#pragma once


#include "comm_system.h"
#include "comm_comp.h"
#include "comm_event.h"
#include "game/scene/game_camera.h"
#include "game/scene/game_scene.h"
#include "game/scene/game_context.h"


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




}