#include "sample_plugin_lua.h"

#include "lua/src/lua.hpp"
#include "luabridge/LuaBridge.h"


namespace samples 
{
    void greeting()
    {
        SPDLOG_INFO("hello, this is C++");
    }

    class Warrior
    {
    public:
        Warrior() = delete;
        Warrior(const std::string& name, int atk, int def)
            : _name(name), _atk(atk), _def(def) {}
        ~Warrior() {}

        const auto& name() { return _name; }
        int atk() { return _atk; }
        int def() { return _def; }
        void desc() { SPDLOG_INFO("warrior: name({}), atk({}), def({})", _name, _atk, _def); }

        void attack(Warrior& enemy) {
            enemy.on_attack(*this);
            on_attack(enemy);
        }

    private:
        void on_attack(Warrior& enemy)
        {
            auto hplost = std::max(enemy.atk() - def(), 0);
            _hp -= hplost;

            if (_hp < 0) {
                SPDLOG_ERROR("{}: dead! killed by {}", name(), enemy.name());
            } else {
                SPDLOG_WARN("{}: hp - {}", name(), hplost);
            }
        }

    private:
        std::string _name;
        int _hp = 100;
        int _atk = 0;
        int _def = 0;
    };


    ImFormLuaSample::ImFormLuaSample(engine::Application* app) : _app(app)
    {
    }
    
    ImFormLuaSample::~ImFormLuaSample()
    {
    }
    
	void ImFormLuaSample::onInit()
	{
        _luaState = luaL_newstate();

        luaL_openlibs(_luaState);


        exportCppFunctions();
	}

    void ImFormLuaSample::onClose()
    {
        lua_close(_luaState);
    }

	void ImFormLuaSample::draw()
	{
        ImGui::Begin("lua");
        {
            if (ImGui::Button("reload test1"))
            {
                greetingLuaBridget();
            }
        }
        ImGui::End();
	}

    void ImFormLuaSample::exportCppFunctions()
    {
        luabridge::getGlobalNamespace(_luaState).addFunction("greeting_in_cpp", greeting);

        luabridge::getGlobalNamespace(_luaState)
            .beginClass<Warrior>("Warrior")
                .addConstructor<void(const std::string&, int, int)>()
                .addFunction("desc", &Warrior::desc)
                .addFunction("attack", &Warrior::attack)
            .endClass();

    }

    void ImFormLuaSample::greetingLuaBridget()
    {
        auto test_file = _app->resPath() / "lua/test_1.lua";
        luaL_dofile(_luaState, test_file.string().c_str());

        auto greet_func = luabridge::getGlobal(_luaState, "greeting_in_lua");
        greet_func.call<>("C-plus-plus");
    }

    ///////////////////////////////////////////////////////////////////////

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