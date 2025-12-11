#include "im_form_demo.h"

#include "engine/application.h"
#include "engine/audio.h"

namespace imgui
{
	void ImFormDemo::draw()
	{
		ImGui::ShowDemoWindow(&_visible);
	}


	
	ImFormTest::ImFormTest()
	{
	}

	ImFormTest::~ImFormTest()
	{

	}

	void ImFormTest::init(engine::Application* app)
	{
 		_application = app;

		auto& audioMgr = _application->resourceManager().audioManager();
		audioMgr.loadMusic("hero", "res/audio/hero_music.mp3");
		audioMgr.loadMusic("win", "res/audio/level_win.mp3");
		audioMgr.loadSound("bow", "res/audio/bow_attack.wav");
	}

	void ImFormTest::draw()
	{
		ImGui::Begin("长风几万里");
		{
			if (ImGui::Button("喝一杯茶")) {
				auto& audioMgr = _application->resourceManager().audioManager();
				audioMgr.playMusic("hero");
			}

			if (ImGui::Button("胜利！")){
				auto& audioMgr = _application->resourceManager().audioManager();
				audioMgr.playMusic("win", true, 1000);
			}

			if (ImGui::Button("大风！")){
				auto& audioMgr = _application->resourceManager().audioManager();
				audioMgr.playSound("bow");
			}
		}
		ImGui::End();
	}
}


