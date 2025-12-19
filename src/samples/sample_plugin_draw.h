#pragma once

#include "engine/application.h"
#include "engine/widget.h"

namespace samples {

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

    private:
        std::vector<engine::Vertex> _vertices;
        engine::Texture* _texture = nullptr;
    };
}