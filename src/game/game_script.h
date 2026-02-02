#pragma once

#include "engine/config.h"

#include "lua/src/lua.hpp"
#include "luabridge/LuaBridge.h"


struct lua_State;


namespace game
{
	using namespace engine;

	class GameScript final
	{
	public:
		GameScript(GameScript&&) = delete;
		GameScript(const GameScript&) = delete;
		
		GameScript();
		~GameScript();

		bool load(const fs::path& filepath);

		bool reload();

		template <typename ResultType, typename... ParamType>
		ResultType call(const std::string& func, ParamType&... params);

	private:
		void exportAll();

	private:
		fs::path _entryPath;
		lua_State* _luaState = nullptr;
	};


	//-----------------------------------------------------------------------

	template <typename ResultType, typename... ParamType>
	ResultType GameScript::call(const std::string& func, ParamType&... params)
	{
		try 
		{
			auto greet_func = luabridge::getGlobal(_luaState, func.c_str());
			if (greet_func.isNil() || !greet_func.isFunction())
			{
				LogCritical("func({}) NOT found in Lua.");
				return ResultType{};
			}

			auto res = greet_func(std::move(params...));
			if (res.isNil())
			{
				LogCritical("func({}) NOT found in Lua.");
				return ResultType{};
			}

			ResultType val = res.cast<ResultType>();
			return val;
		}
		catch (std::exception& e)
		{
			LogCritical("call lua function error, err = {}", e.what());
			throw e;
		}
	}


}