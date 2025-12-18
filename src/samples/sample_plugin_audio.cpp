#include "sample_plugin_audio.h"

namespace samples {
    
	void ImGuiFormAudio::init()
	{
		auto& audioMgr = _application->resourceManager().audioManager();
		audioMgr.loadMusic("audio/hero_music.mp3"_hs);
		audioMgr.loadMusic("audio/level_win.mp3"_hs);
		audioMgr.loadSound("audio/bow_attack.wav"_hs);
	}

	void ImGuiFormAudio::draw()
	{
		ImGui::Begin("长风几万里");
		{
			if (ImGui::Button("喝一杯茶")) {
				_application->audioPlayer().playMusic("audio/hero_music.mp3"_hs);
			}

			if (ImGui::Button("胜利！")) {
				_application->audioPlayer().playMusic("audio/level_win.mp3"_hs, true, 1000);
			}

			if (ImGui::Button("大风！")) {
				_application->audioPlayer().playSound("audio/bow_attack.wav"_hs);
			}
		}
		ImGui::End();
	}

    ///////////////////////////////////////////////////////////////////////


    void SamplePluginAudio::onInit()  
    {
        spdlog::info("Init sample plugin audio");
    
    }

    void SamplePluginAudio::onInstall()  
    {
    }

    void SamplePluginAudio::onUninstall()  
    {
    }

    void SamplePluginAudio::onEnable() 
    {
        imgui::ImFormManager::inst().showForm<ImGuiFormAudio>("ImGuiFormAudio", application());
    }

    void SamplePluginAudio::onDisable() 
    {
        imgui::ImFormManager::inst().closeForm("ImGuiFormAudio");
    }

    void SamplePluginAudio::onUpdate()  
    {
    }

    void SamplePluginAudio::onDraw()  
    {
    }

    void SamplePluginAudio::onClose()  
    {
        spdlog::info("Release sample plugin draw");
    }
}