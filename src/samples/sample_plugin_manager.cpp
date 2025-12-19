#include "sample_plugin_manager.h"
#include "imform/imform_demo.h"

#include "sample_plugin_draw.h"
#include "sample_plugin_audio.h"
#include "sample_plugin_entt.h"
#include "sample_plugin_imgui.h"
#include "sample_plugin_tweeny.h"
#include "sample_plugin_astar.h"
#include "sample_plugin_tilemap.h"

#include "game/game.h"


#include <memory>

namespace samples {

	void ImGuiFormHUD::draw()
	{
		ImGuiIO& io = ImGui::GetIO();
		ImGuiWindowFlags window_flags = 
			ImGuiWindowFlags_NoDecoration | 
			ImGuiWindowFlags_AlwaysAutoResize | 
			ImGuiWindowFlags_NoSavedSettings | 
			ImGuiWindowFlags_NoFocusOnAppearing | 
			ImGuiWindowFlags_NoNav;
			
		const float PAD = 10.0f;
		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImVec2 work_pos = viewport->WorkPos; // Use work area to avoid menu-bar/task-bar, if any!
		ImVec2 work_size = viewport->WorkSize;

		ImVec2 window_pos, window_pos_pivot;
		window_pos.x = work_pos.x + PAD;
		window_pos.y = (work_pos.y + work_size.y - PAD) ;
		window_pos_pivot.x = 0.0f;
		window_pos_pivot.y = 1.0f ;
		ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
		ImGui::SetNextWindowSize({200, 0});

		window_flags |= ImGuiWindowFlags_NoMove;
    	ImGui::SetNextWindowBgAlpha(0.35f); // Transparent background

		auto& fpsChecker = _application->fpsChecker();

		static bool open = true;
		if (ImGui::Begin("Example: Simple overlay", &open, window_flags))
		{
			ImGui::Text("render: ");
			ImGui::Separator();
			
			ImGui::Text("fps: %d", 	 fpsChecker.fps());
			ImGui::Text("avg_fps: %d", fpsChecker.avgFps());
			ImGui::Text("fix_fps: %d", fpsChecker.fixedFps());

			if (ImGui::BeginPopupContextWindow())
			{
				if (open && ImGui::MenuItem("Close")) {
					open = false;
				}
				ImGui::EndPopup();
			}
		}
		ImGui::End();
	}

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
        
        static bool entt_trigger = false;
        if (ImGui::MenuItem("entt", nullptr, &entt_trigger)) 
        {
            SamplePluginManager::inst().setPluginEnable("sample_entt_plugin", entt_trigger);
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
        static bool game_trigger = false;
        if (ImGui::MenuItem("game", nullptr, &game_trigger))
        {
            SamplePluginManager::inst().setPluginEnable("game_logic_plugin", game_trigger);
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

        imgui::ImFormManager::inst().showForm<ImGuiFormHUD>("ImGuiFormHUD", application());
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

        plugin = addNormalPlugin<SamplePluginEntt>();
        plugin->setEnable(false);

        plugin = addNormalPlugin<SamplePluginTweeny>();
        plugin->setEnable(false);

        plugin = addNormalPlugin<SamplePluginAstar>();
        plugin->setEnable(false);

        plugin = addNormalPlugin<SamplePluginTileMap>();
        plugin->setEnable(false);

        plugin = addNormalPlugin<game::GameLogicPlugin>();
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