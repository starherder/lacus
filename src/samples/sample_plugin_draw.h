#pragma once

#include "engine/application.h"

namespace samples {
    class SamplePluginDraw final : public engine::Plugin {
    public:
        SamplePluginDraw() = default;
        ~SamplePluginDraw() = default;

        const char* name() override { 
            return "render_test"; 
        }

        void onInit() override {
            spdlog::info("Init sample plugin draw");
        }

        void onInstall() override {
            spdlog::info("install sample plugin draw");   
        }

        void onUninstall() override {
            spdlog::info("uninstall sample plugin draw");   
        }

        void onUpdate() override {
            //spdlog::info("Update sample render");
        }

        void onDraw() override {

            auto renderer = application()->renderer();
            // 绘制填充矩形
            SDL_FRect fillRect = { 500, 200, 150, 150 };
            renderer->setRenderDrawColor(255, 255, 0, 255);
            renderer->renderFillRect(&fillRect);

            SDL_FRect rect = { 200, 150, 200, 100 };
            renderer->setRenderDrawColor(0, 0, 255, 255);
            renderer->renderRect(&rect);
        }

        void onClose() override {
            spdlog::info("Release sample plugin draw");
        }
    };
}