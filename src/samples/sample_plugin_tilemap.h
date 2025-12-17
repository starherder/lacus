#pragma once

#include "engine/application.h"
#include "imform/imform_manager.h"

#include "tmxmap/TmxMapDemo.h"


namespace samples {

    using namespace engine;

    class SamplePluginTileMap final : public engine::Plugin, 
                                     public utility::sigslot::SlotHandler
    {

    public:
        SamplePluginTileMap() = default;
        ~SamplePluginTileMap() = default;

        const char* name() override { return "sample_tilemap_plugin"; }

        void onInit() override;

        void onInstall() override;

        void onUninstall() override;

        void onEnable() override;

        void onDisable() override;

        void onUpdate() override;

        void onDraw() override;

        void onClose() override;

    private:
        std::unique_ptr<tmx::TmxMapDemo> _tmxMapDemo;
    };
}