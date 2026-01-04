#include "sample_plugin_draw.h"

#include "engine/application.h"
#include "engine/color.h"

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

        paint();
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
                renderer.drawTexture(tex, {0.0f, 0.0f, tex_sz.x, tex_sz.y }, {50.0f, 400.0f, tex_sz.x/2,tex_sz.y/2} );
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


    void SamplePluginDraw::paint()
    {
        auto& painter = application()->painter();


        static float timepassed;
        timepassed += application()->frameTicker().deltaSeconds();
        if(timepassed > 2.0f) 
        { 
            painter.SetAntiAlaised(!painter.IsAntiAlaised());
            timepassed = 0.0f;
        }
        
        bool anti = painter.IsAntiAlaised(); 
        auto font = application()->resourceManager().fontManager().get("fonts/msyh.ttf"_hs, 20);

        painter.DrawRect(Color::PaleBlue, {980, 80, 600, 800}, 5.0f, 3.0f);
        painter.drawText(anti?"aniti_alaised":"", font, {1000, 100});

        painter.DrawCircle(Color::Pink, {1200, 100}, 50, 30, 3);
        painter.FillCircle(Color::LightPink, {1500, 100}, 50, 20);

        painter.DrawLine(Color::DarkCyan, {1000, 150}, {1500, 180}, 5);

        std::vector<Vec2> points = {{1500, 100}, {1550, 200}, {1500, 300}, {1550, 370}};
        painter.DrawLines(Color::Cyan, points.data(), points.size(), false, 1.0f);

        painter.DrawTriangle(Color::PaleRed, { 1000, 240 }, { 1200, 350 }, { 1100, 230 }, 2.0f);
        painter.FillTriangle(Color::DarkRed, { 1000, 380 }, { 1250, 400 }, { 1020, 370 });
        
        painter.FillRect(Color::LightBlue, { 1000, 460, 200, 50}, 8.0f);
        painter.FillRect(Color::DarkBlue, { 1000, 600, 200, 50 });

        points = { {1500, 500}, {1550, 600}, {1500, 500}, {1550, 670} };
        painter.DrawLines(Color::DarkPink, points.data(), points.size(), true, 1.0f);
    }
}