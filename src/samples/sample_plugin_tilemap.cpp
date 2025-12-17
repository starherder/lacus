#include "sample_plugin_tilemap.h"
#include "magic_enum/magic_enum.h"

#include "tmxmap/TmxMapDemo.h"
#include "engine/texture.h"
#include "engine/render.h"


namespace samples {

    void SamplePluginTileMap::onInit()
    {
        _tmxMapDemo = std::make_unique<tmx::TmxMapDemo>(*application());
        _tmxMapDemo->load("scenes/Medieval/test.tmx");
        _tmxMapDemo->setPos({ 50, 50 });
    }

    void SamplePluginTileMap::onInstall()
    {
    }

    void SamplePluginTileMap::onUninstall()
    {
    }

    void SamplePluginTileMap::onEnable()
    {
    }

    void SamplePluginTileMap::onDisable()
    {
    }

    void SamplePluginTileMap::onUpdate()
    {
    }

    void SamplePluginTileMap::onDraw()
    {
        _tmxMapDemo->draw();
    }

    void SamplePluginTileMap::onClose()
    {
    }

}