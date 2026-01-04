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

        gfx_paint();
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
        if (timepassed > 2.0f)
        {
            painter.setAntiAlaised(!painter.isAntiAlaised());
            timepassed = 0.0f;
        }

        bool anti = painter.isAntiAlaised();
        auto font = application()->resourceManager().fontManager().get("fonts/msyh.ttf"_hs, 20);

        painter.drawRect(Color::PaleBlue, { 980, 80, 600, 800 }, 5.0f, 3.0f);
        painter.drawText(anti ? "aniti_alaised" : "", font, { 1000, 100 });

        painter.drawCircle(Color::Pink, { 1200, 150 }, 50, 30, 3);
        painter.fillCircle(Color::LightPink, { 1500, 150 }, 50, 20);

        painter.drawLine(Color::Red, { 1000, 250 }, { 1500, 300 }, 5);

        std::vector<Vec2> points = { {1500, 200}, {1550, 300}, {1500, 400}, {1550, 470} };
        painter.drawLines(Color::Cyan, points.data(), points.size(), false, 1.0f);

        painter.drawTriangle(Color::PaleRed, { 1000, 240 }, { 1200, 350 }, { 1100, 230 }, 2.0f);
        painter.fillTriangle(Color::DarkRed, { 1000, 380 }, { 1250, 400 }, { 1020, 370 });

        painter.fillRect(Color::LightBlue, { 1000, 460, 200, 50 }, 8.0f);
        painter.fillRect(Color::DarkBlue, { 1000, 600, 200, 50 });

        points = { {1500, 500}, {1550, 600}, {1500, 500}, {1550, 670} };
        painter.drawLines(Color::DarkPink, points.data(), points.size(), true, 1.0f);
    }

    void SamplePluginDraw::gfx_paint()
    {
        auto& exp = application()->gfx_painter();

        static float timepassed;
        timepassed += application()->frameTicker().deltaSeconds();
        if (timepassed > 2.0f)
        {
            exp.setAntiAlaised(!exp.isAntiAlaised());
            timepassed = 0.0f;
        }

        Vec2 pos = {100, 650};

        auto text = exp.isAntiAlaised() ? "anti-aliaised" : "";
        auto font = application()->resourceManager().fontManager().get("fonts/VonwaonBitmap-16px.ttf"_hs, 20);
        exp.drawText(text, font, pos + Vec2{200, -20}, Color::Green);

        exp.drawPixel(Color::Red, pos);

        exp.drawArc(Color::Red, pos + Vec2{ 50,50 }, 50, 90, 180);
        exp.drawCircle(Color::LightRed, pos + Vec2{ 110, 50 }, 50);
        exp.drawEllipse(Color::Pink, pos + Vec2{ 50, 110 }, { 100, 50 });

        exp.drawLine(Color::LightCyan, pos + Vec2{ 50, 180 }, pos + Vec2{ 250, 220 });
        exp.drawLine(Color::DarkBlue, pos + Vec2{ 50, 220 }, pos + Vec2{ 250,  180 }, 5);

        exp.drawPie(Color::Yellow, pos + Vec2{ 50, 240 }, 50, 0, 90);

        exp.drawRect(Color::Cyan, { pos + Vec2{ 50, 300 }, Vec2{100, 50} });
        exp.drawRect(Color::Blue, { pos + Vec2{ 50, 360 }, Vec2{100, 50} }, 10);

        exp.drawTriangle(Color::White, pos + Vec2{ 50, 430 }, pos + Vec2{ 100, 430 }, pos + Vec2{ 70, 480 });

        // ----------------------------------------------------------------------------------------------------

        pos = { 400, 650 };

        exp.drawPixel(Color::Red, pos);

        exp.fillCircle(Color::DarkRed, pos + Vec2{ 110, 50 }, 50);
        exp.fillEllipse(Color::Pink, pos + Vec2{ 50, 110 }, { 100, 50 });

        exp.fillPie(Color::Yellow, pos + Vec2{ 50, 240 }, 50, 0, 90);

        exp.fillRect(Color::Cyan, { pos + Vec2{ 50, 300 }, Vec2{100, 50} });
        exp.fillRect(Color::Blue, { pos + Vec2{ 50, 360 }, Vec2{100, 50} }, 10);

        exp.fillTriangle(Color::White, pos + Vec2{ 50, 430 }, pos + Vec2{ 100, 430 }, pos + Vec2{ 70, 480 });
    }
}