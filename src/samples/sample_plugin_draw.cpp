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
        
        renderer.setDrawColor({255, 255, 0, 255});
        renderer.drawFillRect({500, 200, 150, 150});

        renderer.setDrawColor({0, 0, 255, 255});
        renderer.drawRect({ 200, 150, 200, 100 });

        renderer.setDrawColor({255, 155, 0, 255});
        renderer.drawDebugText({10,10}, "Hello,world!");
    }

    void SamplePluginDraw::initGeometry()
    {
        assert(_texture);

        const engine::Vec2i tileCount = { 10, 10 };
        const engine::Vec2i tileSize = { 64, 64 };

        auto textureSize = _texture->size();

        auto uNorm = (float)textureSize.x / 18.0f;
        auto vNorm = (float)textureSize.x / 7.0f;

        const engine::FColor vertColour{ 1,1,1,1 };

        for (int x = 0; x < tileCount.x; x++)
        {
            for (int y = 0; y < tileCount.y; y++)
            {
                float tilePosX = x * tileSize.x;
                float tilePosY = y * tileSize.y;

                float u = uNorm * x;
                float v = uNorm * y;

                //push back to vert array
                engine::Vertex vert = { { tilePosX, tilePosY }, vertColour, {u, v} };
                _vertices.emplace_back(vert);
                vert = { { tilePosX + tileSize.x, tilePosY }, vertColour, {u + uNorm, v} };
                _vertices.emplace_back(vert);
                vert = { { tilePosX, tilePosY + tileSize.y}, vertColour, {u, v + vNorm} };
                _vertices.emplace_back(vert);

                vert = { { tilePosX, tilePosY + tileSize.y}, vertColour, {u, v + vNorm} };
                _vertices.emplace_back(vert);
                vert = { { tilePosX + tileSize.x, tilePosY }, vertColour, {u + uNorm, v} };
                _vertices.emplace_back(vert);
                vert = { { tilePosX + tileSize.x, tilePosY + tileSize.y }, vertColour, {u + uNorm, v + vNorm} };
                _vertices.emplace_back(vert);
            }
        }
    }

    void SamplePluginDraw::drawGeometry()
    {
        if (!_texture)
        {
            auto& textureMgr = application()->resourceManager().textureManager();
            _texture = textureMgr.get("textures/UI/frame.png"_hs);
            initGeometry();
            return;
        }

        auto& renderer = application()->renderer();
        renderer.drawGeometry(_texture, _vertices.data(), _vertices.size(), nullptr, 0);
    }

    void SamplePluginDraw::drawTexture()
    {
        auto& renderer = application()->renderer();
        auto& textureMgr = application()->resourceManager().textureManager();
        {
            auto tex = textureMgr.get("textures/Buildings/house3.png"_hs);
            if(tex)
            {
                auto tex_sz = tex->size();
                renderer.drawTexture(tex, {0, 0, tex_sz.x, tex_sz.y}, {50, 100, tex_sz.x,tex_sz.y} );
            }
            /*
            auto tex2 = textureMgr.get("test");
            if (tex2)
            {
                auto tex_sz = tex2->size();
                renderer.drawTexture(tex2, { 0, 0, tex_sz.x, tex_sz.y }, { 50, 300, tex_sz.x,tex_sz.y });
            }*/
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