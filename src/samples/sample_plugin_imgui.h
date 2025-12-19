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

        void onEnable() override
        {
            imgui::ImFormManager::inst().showForm<imgui::ImGuiFormDemo>("ImGuiFormDemo");
        }

        void onDisable() override
        {
            imgui::ImFormManager::inst().closeForm("ImGuiFormDemo");
        }

    };

}