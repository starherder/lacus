#pragma once

#include "engine/application.h"
#include "engine/painter/sdl_painter.h"
#include "imform/imform.h"

namespace engine {
    class Animation;
}

namespace samples {

    using namespace engine;

    enum class PainterDrawMode
    {
        Sdl,
        Paint,
        Gfx,
        Im
    };

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


    class ImFormTexSet : public imgui::ImForm
    {
    public:
        ImFormTexSet() = delete;
        ImFormTexSet(engine::Application* app, class SamplePluginDraw* plugin)
            : _application(app), _plugin(plugin) {}
        ~ImFormTexSet() = default;

    protected:
        void onInit() override;

        void draw() override;

        void testRotate();

    private:
        engine::Application* _application = nullptr;
        class SamplePluginDraw* _plugin = nullptr;

        std::string _selectTexSet;
        TexTile* _selectTile = nullptr;

        float _scale = 1.0f;
        float _rotate = 0.0f;
        float _round = 0.0f;

        std::vector<const char*> _texSets;
        std::vector<const char*> _texTiles;
    };


    class ImFormAnimSet : public imgui::ImForm
    {
    public:
        ImFormAnimSet() = delete;
        ImFormAnimSet(engine::Application* app, class SamplePluginDraw* plugin)
            : _application(app), _plugin(plugin) {}
        ~ImFormAnimSet() = default;

    protected:
        void onInit() override;
        void draw() override;

    private:
        engine::Application* _application = nullptr;
        class SamplePluginDraw* _plugin = nullptr;

        std::string _selectAnimName;
        engine::Animation* _selectAnim = nullptr;

        std::vector<const char*> _animNames;
        std::vector<std::string> _animNameStore;
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

        PainterDrawMode drawMode() const { return _drawMode; }
        void setDrawMode(PainterDrawMode mode) { _drawMode = mode; }

    private:
        void initGeometry();

        void drawPainterDemo(IPainter& painter, const Vec2& pos);

    private:
        engine::Texture* _texture = nullptr;

        PainterDrawMode _drawMode = PainterDrawMode::Paint;

        std::unique_ptr<SdlPainter> _sdlPainter;
        std::unique_ptr<Painter> _paintPainter;
        std::unique_ptr<GfxPainter> _gfxPainter;
        std::unique_ptr<ImPainter> _imPainter;

        struct VertexData {
            std::vector<Vertex> vertices;
            std::vector<int> indices;
        } _vertexData;
    };
}
