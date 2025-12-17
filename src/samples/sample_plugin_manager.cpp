#include "sample_plugin_manager.h"
#include "imform/imform_demo.h"

#include "sample_plugin_draw.h"
#include "sample_plugin_audio.h"
#include "sample_plugin_scene.h"
#include "sample_plugin_imgui.h"
#include "sample_plugin_tweeny.h"
#include "sample_plugin_astar.h"
#include "sample_plugin_tilemap.h"



#include <memory>

namespace samples {

    void ImFormMainFrame::draw()
    {
        drawMainMenu();
    }

    void ImFormMainFrame::drawMainMenu()
    {
        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("Plugins")) {
                drawMenuPlugins();
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("help")) {
                if (ImGui::MenuItem("about"))  {
                }
                ImGui::EndMenu();
            }

            ImGui::EndMainMenuBar();
        }
    }
    
    void ImFormMainFrame::drawMenuPlugins()
    {
        static bool draw_trigger = false;
        if (ImGui::MenuItem("draw", nullptr, &draw_trigger)) 
        {
            SamplePluginManager::inst().setPluginEnable("sample_draw_plugin", draw_trigger);
        }
        
        static bool imgui_trigger = false;
        if (ImGui::MenuItem("imgui", nullptr, &imgui_trigger)) 
        {
            SamplePluginManager::inst().setPluginEnable("sample_imgui_plugin", imgui_trigger);
        }
        
        static bool audio_trigger = false;
        if (ImGui::MenuItem("audio", nullptr, &audio_trigger)) 
        {
            SamplePluginManager::inst().setPluginEnable("sample_audio_plugin", audio_trigger);
        }
        
        static bool scene_trigger = false;
        if (ImGui::MenuItem("scene", nullptr, &scene_trigger)) 
        {
            SamplePluginManager::inst().setPluginEnable("sample_scene_plugin", scene_trigger);
        }
        static bool tweeny_trigger = false;
        if (ImGui::MenuItem("tweeny", nullptr, &tweeny_trigger)) 
        {
            SamplePluginManager::inst().setPluginEnable("sample_tweeny_plugin", tweeny_trigger);
        }
        static bool astar_trigger = false;
        if (ImGui::MenuItem("pathfind", nullptr, &astar_trigger))
        {
            SamplePluginManager::inst().setPluginEnable("sample_astar_plugin", astar_trigger);
        }
        static bool tilemap_trigger = false;
        if (ImGui::MenuItem("tilemap", nullptr, &tilemap_trigger))
        {
            SamplePluginManager::inst().setPluginEnable("sample_tilemap_plugin", tilemap_trigger);
        }
    }

    // ----------------------------------------------------------------------------------------

    SamplePluginMain::SamplePluginMain(engine::Application& app) : _app(app)
    {
    }

    void SamplePluginMain::onInit()
    {
        spdlog::info("SamplePluginMain::onInit");
    
        auto window = _app.window().getSdlWindow();
        auto renderer = _app.renderer().getSdlRenderer();

        imgui::ImFormManager::inst().init(window, renderer);

        auto font_path = application()->resPath()/"fonts/msyh.ttf";
        imgui::ImFormManager::inst().setFont(font_path, 20);

        imgui::ImFormManager::inst().showForm<ImFormMainFrame>("ImFormMainFrame");

        imgui::ImFormManager::inst().showForm<imgui::ImGuiFormHUD>("ImGuiFormHUD", application());
    }

    void SamplePluginMain::onClose()
    {
        imgui::ImFormManager::inst().closeForm("ImGuiFormHUD");
        spdlog::info("SamplePluginMain::onClose");
    }

    void SamplePluginMain::onUpdate()
    {
    }

    void SamplePluginMain::onEvent(const engine::Event& event) 
    {
        imgui::ImFormManager::inst().processEvent(event);
    }

    void SamplePluginMain::onDraw() 
    {
    }

    void SamplePluginMain::onDrawUI()
    {
        imgui::ImFormManager::inst().draw();
    }

    // ----------------------------------------------------------------------------------------

    void SamplePluginManager::init(engine::Application& app)
    {
        spdlog::info("SamplePluginManager::init");

        _app = &app;

        engine::Plugin* plugin = nullptr;
        plugin = addRootPlugin<SamplePluginMain>(app);
        
        plugin = addNormalPlugin<SamplePluginDraw>();
        plugin->setEnable(false);

        plugin = addNormalPlugin<SamplePluginImgui>();
        plugin->setEnable(false);

        plugin = addNormalPlugin<SamplePluginAudio>();
        plugin->setEnable(false);

        plugin = addNormalPlugin<SamplePluginScene>();
        plugin->setEnable(false);

        plugin = addNormalPlugin<SamplePluginTweeny>();
        plugin->setEnable(false);

        plugin = addNormalPlugin<SamplePluginAstar>();
        plugin->setEnable(false);

        plugin = addNormalPlugin<SamplePluginTileMap>();
        plugin->setEnable(false);
    }

    bool SamplePluginManager::removePlugin(const std::string& name)
    {
        spdlog::info("SamplePluginManager::removePlugin {}", name);
        _app->removePlugin(name);
        return true;
    }
    
    void SamplePluginManager::setPluginEnable(const std::string& name, bool enabled)
    {
           auto plugin = _app->getPlugin(name);
           if(!plugin)
           {
                spdlog::error("plugin ({}) NOT found.", name);
                return;
           }

           plugin->setEnable(enabled);
    }

}