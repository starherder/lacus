#include "game_script.h"
#include "engine/wrapper.h"
#include "nlohmann/json.hpp"

#include "game/ecs/comm_comp.h"
#include "game/ecs/comp_fight.h"


namespace game
{
	static void log_trace(const char* str) { LogTrace(str); }
	static void log_debug(const char* str) { LogDebug(str); }
	static void log_info(const char* str) { LogInfo(str); }
	static void log_warn(const char* str) { LogWarn(str); }
	static void log_error(const char* str) { LogError(str); }
	static void log_critical(const char* str) { LogCritical(str); }

	//-----------------------------------------------------------------------

	GameScript::GameScript() 
	{
		_luaState = luaL_newstate();
		luaL_openlibs(_luaState);
	
		exportAll();
	}

	GameScript::~GameScript() 
	{
	}

	bool GameScript::load(const fs::path& filepath)
	{
		LogInfo("GameScript::load: script = {}", filepath.string());

		_entryPath = filepath;

		return luaL_dofile(_luaState, filepath.string().c_str());
	}

	bool GameScript::reload()
	{
		return load(_entryPath);
	}

	void GameScript::exportAll()
	{
		luabridge::getGlobalNamespace(_luaState)
			.addFunction("log_trace", log_trace)
			.addFunction("log_debug", log_debug)
			.addFunction("log_info", log_info)
			.addFunction("log_warn", log_warn)
			.addFunction("log_error", log_error)
			.addFunction("log_critical", log_critical)
			;

		luabridge::getGlobalNamespace(_luaState)
			.beginClass<CompBaseProp>("BaseProps")
				.addConstructor<void(*)()>()
				.addData("lv",  &CompBaseProp::lv)
				.addData("exp", &CompBaseProp::exp)
				.addData("str", &CompBaseProp::str)
				.addData("cst", &CompBaseProp::cst)
				.addData("dex", &CompBaseProp::dex)
				.addData("met", &CompBaseProp::met)
			.endClass();

		luabridge::getGlobalNamespace(_luaState)
			.beginClass<CompFightProp>("FightProps")
				.addConstructor<void(*)()>()
				.addData("hp",  &CompFightProp::hp)
				.addData("hpm", &CompFightProp::hpm)
				.addData("hpr", &CompFightProp::hpr)
				.addData("atk", &CompFightProp::atk)
				.addData("def", &CompFightProp::def)
				.addData("mvs", &CompFightProp::mvs)
				.addData("ats", &CompFightProp::ats)
				.addData("atd", &CompFightProp::atd)
				.addData("crt", &CompFightProp::crt)
				.addData("par", &CompFightProp::par)
			.endClass();
	}

}