#include "sample_plugin_draw.h"

#include "engine/application.h"

namespace samples {

    void SamplePluginDraw::onInit() 
    {
    }

    void SamplePluginDraw::onInstall() 
    {  
    }

    void SamplePluginDraw::onUninstall() 
    {
    }

    void SamplePluginDraw::onEnable()
    {
    }

    void SamplePluginDraw::onDisable()
    {
    }

    void SamplePluginDraw::onUpdate() 
    {
    }

    void SamplePluginDraw::onDraw() 
    {
        drawTexture();

        drawShape();

        drawText();

        drawGeometry();
    }

    void SamplePluginDraw::onClose() 
    {
        spdlog::info("Release sample plugin draw");
    }

    void SamplePluginDraw::drawShape()
    {
        auto& renderer = application()->renderer();
        
        renderer.setDrawColor({0, 0, 255, 255});
        renderer.drawRect({ 100, 100, 200, 100 });

        renderer.setDrawColor({255, 255, 0, 255});
        renderer.drawFillRect({400, 100, 150, 150});

        renderer.setDrawColor({255, 155, 0, 255});
        renderer.drawDebugText({10,10}, "Hello,world!");
    }

    void SamplePluginDraw::initGeometry()
    {
    }

    void SamplePluginDraw::drawGeometry()
    {
    }

    void SamplePluginDraw::drawTexture()
    {
        auto& renderer = application()->renderer();
        auto& textureMgr = application()->resourceManager().textureManager();
        {
            auto tex = textureMgr.get("textures/UI/title.png"_hs);
            if(tex)
            {
                auto tex_sz = tex->size();
                renderer.drawTexture(tex, {0.0f, 0.0f, tex_sz.x, tex_sz.y}, {50.0f, 400.0f, tex_sz.x/2,tex_sz.y/2} );
            }
        }
    }

    void SamplePluginDraw::drawText()
    {
        auto& renderer = application()->renderer();
        auto& resourceMgr = application()->resourceManager();
        {
            auto font = resourceMgr.fontManager().get("fonts/VonwaonBitmap-16px.ttf"_hs, 30);
            if(font)
            {
                renderer.drawText("WTF! 妙~", font, {800, 100}, {255, 100, 0, 255});
            }
        }
        {
            auto font = resourceMgr.fontManager().get("fonts/msyh.ttf"_hs, 20);
            if(font)
            {
                renderer.drawText("就是这个feel，倍er爽~", font, {800, 200}, {255, 200, 0, 255});
            }
        }
    }
}