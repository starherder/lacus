#pragma once

#include "engine/plugin.h"
#include "engine/application.h"

#include "imform/imform_manager.h"
#include "imform/imform_demo.h"

namespace samples {

    class SamplePluginImgui final : public engine::Plugin {
    public:
        SamplePluginImgui() = default;
        ~SamplePluginImgui() = default;

        const char* name() override { return "sample_imgui_plugin"; }

        void onInit() override 
        {
        }

        void onEnable() override
        {
            imgui::ImFormManager::inst().showForm<imgui::ImGuiFormDemo>("ImGuiFormDemo");
            imgui::ImFormManager::inst().showForm<imgui::ImGuiFormHUD>("ImGuiFormHUD", application());
        }

        void onDisable() override
        {
            imgui::ImFormManager::inst().closeForm("ImGuiFormHUD");
            imgui::ImFormManager::inst().closeForm("ImGuiFormDemo");
        }

        void onEvent(const engine::Event& event) 
        {
        }

        void onUpdate() override 
        {
        }

        void onDraw() override 
        {
        }

        void onInstall() override 
        {
        }

        void onUninstall() override 
        {
        }

        void onClose() override 
        {
            spdlog::info("close imgui plugin ");
        }
    };

}