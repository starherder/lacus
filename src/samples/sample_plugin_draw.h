#pragma once

#include "engine/application.h"
#include "imform/imform.h"

namespace samples {

    using namespace engine;

    class ImGuiFormDraw : public imgui::ImForm
    {
    public:
        ImGuiFormDraw() = delete;
        ImGuiFormDraw(engine::Application* app) : _application(app) {}
        ~ImGuiFormDraw() = default;

    protected:
        void onInit() override;

        void draw() override;

    private:
        engine::Application* _application = nullptr;
    };




    class SamplePluginDraw final : public engine::Plugin 
    {
    public:
        SamplePluginDraw() = default;
        ~SamplePluginDraw() = default;

        const char* name() override { return "sample_draw_plugin"; }

        void onInit() override ;

        void onInstall() override ;

        void onUninstall() override ;

        void onEnable() override;

        void onDisable() override;

        void onUpdate() override ;

        void onDraw() override ;

        void onClose() override ;

    private:
        void drawShape();

        void initGeometry();

        void drawGeometry();

        void drawTexture();

        void drawText();

        void drawImGuiCmds();

        void paint();

        void gfx_paint();

    private:
        std::vector<engine::Vertex> _vertices;
        engine::Texture* _texture = nullptr;
    };
}