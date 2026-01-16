#pragma once

#include "engine/application.h"
#include "imform/imform.h"

namespace samples {

    using namespace engine;

    class ImGuiFormDraw : public imgui::ImForm
    {
    public:
        ImGuiFormDraw() = delete;
        ImGuiFormDraw(engine::Application* app, class SamplePluginDraw* plugin) 
            : _application(app), _plugin(plugin) {}
        ~ImGuiFormDraw() = default;

    protected:
        void onInit() override;

        void draw() override;

    private:
        engine::Application* _application = nullptr;
        class SamplePluginDraw* _plugin = nullptr;
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

        void setImDraw(bool visible) { _im_draw = visible; }

    private:
        void drawShape();

        void initGeometry();

        void drawGeometry();

        void drawTexture();

        void drawText();

        void im_paint();

        void paint();

        void gfx_paint();

    private:
        engine::Texture* _texture = nullptr;

        bool _im_draw = false;

        struct VertexData {
            std::vector<Vertex> vertices;
            std::vector<int> indices;
        } _vertexData;

    };
}