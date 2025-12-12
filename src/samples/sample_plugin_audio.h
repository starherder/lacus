#pragma once

#include "engine/application.h"
#include "imform/im_manager.h"
#include "imform/im_form_demo.h"

namespace samples {

    class SamplePluginAudio final : public engine::Plugin 
    {
    public:
        SamplePluginAudio() = default;
        ~SamplePluginAudio() = default;

        const char* name() override 
        { 
            return "audio_test"; 
        }

        void onInit() override 
        {
            spdlog::info("Init sample plugin audio");
        }

        void onInstall() override 
        {
            spdlog::info("install sample plugin audio");   
        }

        void onUninstall() override 
        {
            spdlog::info("uninstall sample plugin audio");   
        }

        void onUpdate() override 
        {
            if(_frame++ == 10)
            {
                auto pForm = std::dynamic_pointer_cast<imgui::ImFormAudio>(imgui::ImGuiManager::inst().showForm<imgui::ImFormAudio>("ImFormAudio"));
                if(pForm)
                {
                    pForm->init(application());
                }
            }
        }

        void onDraw() override 
        {
        }

        void onClose() override 
        {
            spdlog::info("Release sample plugin draw");
        }

    private:
        uint64_t _frame = 0;
    };
}