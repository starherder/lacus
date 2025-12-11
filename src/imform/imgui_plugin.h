#pragma once

#include "engine/application.h"
#include "im_manager.h"

#include "im_form_demo.h"

namespace imgui {

    class ImGuiPlugin final : public engine::Plugin {
    public:
        ImGuiPlugin() = default;
        ~ImGuiPlugin() = default;

        const char* name() override { 
            return "imgui_plugin"; 
        }

        void onInit() override {
            spdlog::info("Init imgui plugin ");

            auto window = application()->window().getSdlWindow();
            auto renderer = application()->renderer().getSdlRenderer();

            ImGuiManager::inst().init(window, renderer);

            auto font_path = application()->resPath()/"fonts/msyh.ttf";
            ImGuiManager::inst().setFont(font_path, 20);
            //ImGuiManager::inst().setScale(2);
            
            ImGuiManager::inst().showForm<ImFormDemo>("ImFormDemo");
            ImGuiManager::inst().showForm<ImFormTest>("ImFormTest");
        }

        void onInstall() override {
            spdlog::info("install imgui plugin");   
        }

        void onUninstall() override {
            spdlog::info("uninstall imgui plugin");   
        }
        
        void onEvent(engine::Event& event) {
            ImGuiManager::inst().processEvent(event);
        }

        void onUpdate() override {
            //spdlog::info("Update imgui plugin");
        }

        void onDraw() override {
            ImGuiManager::inst().draw();
        }

        void onClose() override {
            spdlog::info("Release imgui plugin");
        }
    };

}