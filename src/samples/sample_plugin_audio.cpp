#include "sample_plugin_audio.h"

namespace samples 
{
    
	void ImGuiFormAudio::onInit()
	{
		auto& audioMgr = _application->resourceManager().audioManager();
		audioMgr.loadMusic("audio/hero_music.mp3"_hs);
		audioMgr.loadMusic("audio/level_win.mp3"_hs);
		audioMgr.loadSound("audio/bow_attack.wav"_hs);

        std::filesystem::path audioPath = _application->resPath() / "audio";
        for (const auto& entry : std::filesystem::recursive_directory_iterator(audioPath)) 
        {
             if (entry.is_regular_file()) 
             {
                 auto audioname = entry.path().lexically_relative(_application->resPath());
                 _audioNames.push_back(audioname.string());
             }
        }

        for(auto& name : _audioNames)
        {
            _audioList.push_back(name.c_str());
        }
	}

	void ImGuiFormAudio::draw()
	{
        ImGui::Begin("audios");
        {
            const ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowSize({ viewport->Size.x, 500});

            static bool music_start = false;
            static int sound_channel = -1;
            static std::string audio_name;

            static int listbox_item_current = 0;
            if (ImGui::ListBox("##audio_list", &listbox_item_current, _audioList.data(), (int)_audioList.size()))
            {
                audio_name = _audioList[listbox_item_current];
                sound_channel = _application->audioPlayer().playSound(engine::HashString(audio_name.c_str()));

                music_start = true;
            }

            auto volume = _application->audioPlayer().getSoundVolume();
            if (ImGui::SliderFloat("volumn##audio", &volume, 0.0f, 1.0f))
            {
                _application->audioPlayer().setSoundVolume(volume);
            }

            if (ImGui::Button(music_start ? "stop" : "start"))
            {
                if (music_start) {
                    _application->audioPlayer().stopSound(sound_channel);
                }
                else {
                    sound_channel = _application->audioPlayer().playSound(engine::HashString(audio_name.c_str()));
                }
                music_start = !music_start;
            }
        }
        ImGui::End();
	}

    ///////////////////////////////////////////////////////////////////////


    void SamplePluginAudio::onInit()  
    {
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
        LogInfo("Release sample plugin draw");
    }
}