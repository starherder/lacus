#include "sample_plugin_lua.h"

#include "lua/src/lua.hpp"
#include "luabridge/LuaBridge.h"


namespace samples 
{
    
	void ImFormLuaSample::onInit()
	{
	}

	void ImFormLuaSample::draw()
	{
        ImGui::Begin("lua");
        {
        }
        ImGui::End();
	}

    ///////////////////////////////////////////////////////////////////////

    void greeting()
    {
        SPDLOG_INFO("hello, this is C++");
    }

    void SamplePluginLua::greetingLuaBridget()
    {
        lua_State* L = luaL_newstate();
        luaL_openlibs(L);

        luabridge::getGlobalNamespace(L).addFunction("greeting_in_cpp", greeting);

        auto test_file = application()->resPath() / "lua/test_1.lua";
        luaL_dofile(L, test_file.string().c_str());
    }

    void SamplePluginLua::onInit()  
    {
    }

    void SamplePluginLua::onInstall()  
    {
    }

    void SamplePluginLua::onUninstall()  
    {
    }

    void SamplePluginLua::onEnable() 
    {
        imgui::ImFormManager::inst().showForm<ImFormLuaSample>("ImFormLuaSample", application());

        greetingLuaBridget();
    }

    void SamplePluginLua::onDisable() 
    {
        imgui::ImFormManager::inst().closeForm("ImFormLuaSample");
    }

    void SamplePluginLua::onUpdate()  
    {
    }

    void SamplePluginLua::onDraw()  
    {
    }

    void SamplePluginLua::onClose()  
    {
    }
}