#include "im_form_demo.h"

#include "engine/application.h"
#include "engine/audio.h"

namespace imgui
{
	void ImFormDemo::draw()
	{
		ImGui::ShowDemoWindow(&_visible);
	}

	////////////////////////////////////////////////////////////////////////////////////////////

	void ImFormAudio::init(engine::Application* app)
	{
 		_application = app;

		auto& audioMgr = _application->resourceManager().audioManager();
		audioMgr.loadMusic("hero", "res/audio/hero_music.mp3");
		audioMgr.loadMusic("win", "res/audio/level_win.mp3");
		audioMgr.loadSound("bow", "res/audio/bow_attack.wav");
	}

	void ImFormAudio::draw()
	{
		ImGui::Begin("长风几万里");
		{
			if (ImGui::Button("喝一杯茶")) {
				_application->audioPlayer().playMusic("hero");
			}

			if (ImGui::Button("胜利！")) {
				_application->audioPlayer().playMusic("win", true, 1000);
			}

			if (ImGui::Button("大风！")) {
				_application->audioPlayer().playSound("bow");
			}
		}
		ImGui::End();
	}
	
	////////////////////////////////////////////////////////////////////////////////////////////

	void ImFormHUD::draw()
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
			ImGui::Text("avg_fps: %d", fpsChecker.avg_fps());
			ImGui::Text("fix_fps: %d", fpsChecker.fixed_fps());

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
}


