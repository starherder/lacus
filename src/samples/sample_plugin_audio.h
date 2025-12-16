#pragma once

#include "engine/application.h"
#include "imform/imform_manager.h"
#include "imform/imform_demo.h"

namespace samples {

    class SamplePluginAudio final : public engine::Plugin 
    {
    public:
        SamplePluginAudio() = default;
        ~SamplePluginAudio() = default;

        const char* name() override { return "sample_audio_plugin"; }

        void onInit() override 
        {
            spdlog::info("Init sample plugin audio");
        
        }

        void onInstall() override 
        {
        }

        void onUninstall() override 
        {
        }

        void onEnable() override
        {
            imgui::ImFormManager::inst().showForm<imgui::ImGuiFormAudio>("ImGuiFormAudio", application());
        }

        void onDisable() override
        {
            imgui::ImFormManager::inst().closeForm("ImGuiFormAudio");
        }

        void onUpdate() override 
        {
        }

        void onDraw() override 
        {
        }

        void onClose() override 
        {
            spdlog::info("Release sample plugin draw");
        }

    };
}