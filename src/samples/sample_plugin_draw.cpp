#include "sample_plugin_draw.h"

#include "engine/application.h"
#include "engine/color.h"

#include "imform/imform_manager.h"

namespace samples {

    void ImGuiFormDraw::onInit()
    {
    }

    void ImGuiFormDraw::draw()
    {
        ImGuiIO& io = ImGui::GetIO();

        ImGui::Begin("drawlist");

        static bool im_visible = false;
        if (ImGui::Checkbox("im_paint", &im_visible))
        {
            _plugin->setImDraw(im_visible);
        }

        ImGui::SameLine();

        static bool sdl_visible = false;
        if (ImGui::Checkbox("sdl_paint", &sdl_visible))
        {
            _plugin->setSdlDraw(sdl_visible);
        }

        ImGui::SameLine();

        static bool paint_visible = false;
        if (ImGui::Checkbox("base_paint", &paint_visible))
        {
            _plugin->setPaintDraw(paint_visible);
        }

        ImGui::SameLine();

        static bool gfx_visible = false;
        if (ImGui::Checkbox("gfx_paint", &gfx_visible))
        {
            _plugin->setGfxDraw(gfx_visible);
        }

        ImVec2 pos = ImGui::GetWindowPos();

        // 注意ImGUI中的ImU32格式的颜色顺序是ABGR，见ImGui::ColorConvertFloat4ToU32

        ImGui::GetWindowDrawList()->AddRectFilled({ pos.x + 100, pos.y + 100 }, { pos.x + 300, pos.y + 200 }, ImColor(40, 240, 50, 255));
        ImGui::GetWindowDrawList()->AddRectFilled({ pos.x + 100, pos.y + 210 }, { pos.x + 300, pos.y + 310 }, ImColor(140, 240, 150, 255), 10);
        ImGui::GetWindowDrawList()->AddRectFilled({ pos.x + 310, pos.y + 100 }, { pos.x + 510, pos.y + 200 }, ImColor(140, 150, 240, 255), 10);
        //ImGui::GetWindowDrawList()->AddRectFilled({ pos.x + 310, pos.y + 100 }, { pos.x + 510, pos.y + 200 }, 0x0000FFFF, 10); // ABGR,  A==0, 绘制不出来
        ImGui::GetWindowDrawList()->AddRect({ pos.x + 310, pos.y + 210 }, { pos.x + 510, pos.y + 310 }, ImColor(40, 240, 50, 255),10, ImDrawFlags_RoundCornersAll, 10);

        ImGui::GetWindowDrawList()->AddCircle({ pos.x + 200, pos.y + 450 }, 80, ImColor{ 240, 40, 50, 255 }, 10);
        ImGui::GetWindowDrawList()->AddCircle({ pos.x + 420, pos.y + 450 }, 80, ImColor{ 240, 140, 150, 255 }, 50, 10.0f);
        ImGui::GetWindowDrawList()->AddCircleFilled({ pos.x + 200, pos.y + 660 }, 80, ImColor{ 240, 140, 150, 255 }, 20);
        ImGui::GetWindowDrawList()->AddCircleFilled({ pos.x + 420, pos.y + 660 }, 80, ImColor{ 240, 240, 150, 255 }, 20);

        ImGui::End();

        // ------------------------------------------------------------

        pos = ImVec2{ 1900, 200 };

        ImGui::GetBackgroundDrawList()->AddRectFilled({ pos.x + 100, pos.y + 100 }, { pos.x + 300, pos.y + 200 }, ImColor(40, 240, 50, 255));
        ImGui::GetBackgroundDrawList()->AddRectFilled({ pos.x + 100, pos.y + 210 }, { pos.x + 300, pos.y + 310 }, ImColor(140, 240, 150, 255), 10);
        ImGui::GetBackgroundDrawList()->AddRectFilled({ pos.x + 310, pos.y + 100 }, { pos.x + 510, pos.y + 200 }, ImColor(140, 150, 240, 255), 10);
        ImGui::GetBackgroundDrawList()->AddRect({ pos.x + 310, pos.y + 210 }, { pos.x + 510, pos.y + 310 }, ImColor(40, 240, 50, 255), 10, ImDrawFlags_RoundCornersAll, 10);

        ImGui::GetBackgroundDrawList()->AddCircle({ pos.x + 200, pos.y + 450 }, 80, ImColor{ 240, 40, 50, 255 }, 10);
        ImGui::GetBackgroundDrawList()->AddCircle({ pos.x + 420, pos.y + 450 }, 80, ImColor{ 240, 140, 150, 255 }, 50, 10.0f);
        ImGui::GetBackgroundDrawList()->AddCircleFilled({ pos.x + 200, pos.y + 660 }, 80, ImColor{ 240, 140, 150, 255 }, 20);
        ImGui::GetBackgroundDrawList()->AddCircleFilled({ pos.x + 420, pos.y + 660 }, 80, ImColor{ 240, 140, 250, 255 }, 20);

        // ------------------------------------------------------------

        pos = ImVec2{ 1400, 600 };

        ImGui::GetForegroundDrawList()->AddRectFilled({ pos.x + 100, pos.y + 100 }, { pos.x + 300, pos.y + 200 }, ImColor(40, 240, 50, 255));
        ImGui::GetForegroundDrawList()->AddRectFilled({ pos.x + 100, pos.y + 210 }, { pos.x + 300, pos.y + 310 }, ImColor(140, 240, 150, 255), 10);
        ImGui::GetForegroundDrawList()->AddRectFilled({ pos.x + 310, pos.y + 100 }, { pos.x + 510, pos.y + 200 }, ImColor(140, 240, 250, 255), 10);
        ImGui::GetForegroundDrawList()->AddRect({ pos.x + 310, pos.y + 210 }, { pos.x + 510, pos.y + 310 }, ImColor(40, 240, 50, 255), 10, ImDrawFlags_RoundCornersAll, 10);

        ImGui::GetForegroundDrawList()->AddCircle({ pos.x + 200, pos.y + 450 }, 80, ImColor{ 240, 40, 50, 255 }, 10);
        ImGui::GetForegroundDrawList()->AddCircle({ pos.x + 420, pos.y + 450 }, 80, ImColor{ 240, 140, 150, 255 }, 50, 10.0f);
        ImGui::GetForegroundDrawList()->AddCircleFilled({ pos.x + 200, pos.y + 660 }, 80, ImColor{ 240, 140, 150, 255 }, 20);
        ImGui::GetForegroundDrawList()->AddCircleFilled({ pos.x + 420, pos.y + 660 }, 80, ImColor{ 240, 140, 250, 255 }, 20);
    }

    void SamplePluginDraw::onInit() 
    {
        application()->resourceManager().textureManager().loadTexSet("textures/ts_role.xml");

        initGeometry();
    }

    void SamplePluginDraw::onInstall() 
    {  
    }

    void SamplePluginDraw::onUninstall() 
    {
    }

    void SamplePluginDraw::onEnable()
    {
        imgui::ImFormManager::inst().showForm<ImGuiFormDraw>("form_draw", application(), this);
    }

    void SamplePluginDraw::onDisable()
    {
        imgui::ImFormManager::inst().closeForm("form_draw");
    }

    void SamplePluginDraw::onUpdate() 
    {
    }

    void SamplePluginDraw::onDraw() 
    {
        if(_sdl_draw)
        {
            drawTexture();

            drawShape();

            drawText();
        }

        if(_paint_draw)
        {
            paint();
        }

        if(_gfx_draw)
        {
            gfx_paint();
        }

        if(_im_draw)
        {
            drawGeometry();

            im_paint();
        }
    }

    void SamplePluginDraw::onClose() 
    {
        LogInfo("Release sample plugin draw");
    }

    void SamplePluginDraw::drawShape()
    {
        auto& renderer = application()->renderer();
        auto& painter = application()->painter();
        //auto& gfxPainter = application()->gfx_painter();

        renderer.setDrawColor({0, 0, 255, 255});
        renderer.drawRect({ 100, 100, 200, 100 });
        renderer.setDrawColor({255, 255, 0, 255});
        renderer.drawFillRect({400, 100, 150, 150});

        renderer.setDrawColor({255, 155, 0, 255});
        renderer.drawDebugText({10,10}, "Hello,world!");

        // ----------------------------------------------------

        renderer.setDrawColor(Color::Red);
        renderer.drawRect({1700, 50, 200, 100});
        
        painter.drawRect(Color::Blue, { 1700, 160, 200, 100 }, 10);
        painter.drawRect(Color::Green, { 1700, 270, 200, 100 }, 10, 10);
        
       // gfxPainter.drawRect(Color::Pink, {1700, 380, 200, 100 }, 10);
        //gfxPainter.drawRect(Color::Pink, { 1910, 50, 200, 100 }, 10);
    }

    void SamplePluginDraw::initGeometry()
    {
        std::string imagename = "textures/UI/frame.png";
        _texture = application()->resourceManager().textureManager().load(HashString{ imagename.c_str() });
        if (!_texture)
        {
            LogError("load texture {} failed", imagename);
            return;
        }

        int _xcount = 30, _ycount = 30;
        float _gridw = 50.0f, _gridh = 50.0f;

        for (int x = 0; x < _xcount; x++)
        {
            for (int y = 0; y < _ycount; y++)
            {
                auto pos = Vec2f{ x * _gridw, y * _gridh };
                auto size = Vec2f{ _gridw, _gridh };

                auto color = Color{ HSVColor{(float)(x % 360), 1.0f, 1.0f, 1.0f } };

                Vertex v1{ {pos.x, pos.y},                   color, {0.0f,      0.0f} };
                Vertex v2{ {pos.x + size.x, pos.y},          color, {0.333333f, 0.0f} };
                Vertex v3{ {pos.x + size.x, pos.y + size.y}, color, {0.333333f, 1.0f} };
                Vertex v4{ {pos.x, pos.y + size.y},          color, {0.0f,      1.0f} };

                _vertexData.vertices.push_back(v1);
                _vertexData.vertices.push_back(v2);
                _vertexData.vertices.push_back(v3);
                _vertexData.vertices.push_back(v4);

                _vertexData.indices.push_back((x * _xcount + y) * 4 + 0);
                _vertexData.indices.push_back((x * _xcount + y) * 4 + 1);
                _vertexData.indices.push_back((x * _xcount + y) * 4 + 2);
                _vertexData.indices.push_back((x * _xcount + y) * 4 + 0);
                _vertexData.indices.push_back((x * _xcount + y) * 4 + 2);
                _vertexData.indices.push_back((x * _xcount + y) * 4 + 3);
            }
        }


        auto& painter = application()->painter();

        float dis = 200.0f;
        Vec2 center =  Vec2{ 1500, 300 };
        for (float r = 0; r < 2 * PI; r += 0.5f)
        {
            Vec2 dst = center + Vec2{ sin(r) * dis, cos(r) * dis };

            std::vector<Vec2> data;
            painter.makeLightningData(data, center, dst, 100);

            _lightning.push_back(data);
        }
    }

    void SamplePluginDraw::drawGeometry()
    {
        if (!_im_draw)
        {
            return;
        }

        std::string imagename = "textures/UI/frame.png";
        auto texture = application()->resourceManager().textureManager().get(HashString{ imagename.c_str() });

        auto& painter = application()->painter();
        painter.drawGeometry(texture, _vertexData.vertices.data(), _vertexData.vertices.size(),
                                        _vertexData.indices.data(), _vertexData.indices.size());
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

            auto svgTex = textureMgr.get("textures/UI/tiger.svg"_hs);
            if (svgTex)
            {
                auto tex_sz = svgTex->size();
                renderer.drawTexture(svgTex, { 0.0f, 0.0f, tex_sz.x, tex_sz.y }, { 400.0f, 300.0f, tex_sz.x/2, tex_sz.y/2 });
            }
        }
    }

    void SamplePluginDraw::drawText()
    {
        auto& renderer = application()->renderer();
        auto& resourceMgr = application()->resourceManager();
        {
            auto font = resourceMgr.fontManager().get("fonts/Vonwaon.ttf"_hs, 30);
            if(font)
            {
                renderer.drawText("WTF! 妙~", font, {650, 100}, {255, 100, 0, 255});
            }
        }
        {
            auto font = resourceMgr.fontManager().get("fonts/msyh.ttf"_hs, 20);
            if(font)
            {
                renderer.drawText("就是这个feel，倍er爽~", font, {650, 150}, {255, 200, 0, 255});
            }
        }
    }

    void SamplePluginDraw::im_paint()
    {
        if (!_im_draw)
        {
            return;
        }

        auto& painter = application()->painter();
        auto& textureMgr = application()->resourceManager().textureManager();
        //application()->renderer().setClearColor(Color::White);

        Vec2 pos = {600, 300};

        painter.drawCircle(Color::Red, pos + Vec2{ 150, 0 }, 50, 30, 6);
        painter.drawCircle(Color::Green, pos + Vec2{ 260, 0 }, 50, 30, 6);
        painter.drawCircle(Color::Blue, pos + Vec2{ 370, 0 }, 50, 30, 6);

        painter.fillCircle(Color::Yellow, pos + Vec2{ 150, 100 }, 50, 20);
        painter.fillCircle(Color::Cyan, pos + Vec2{ 260, 100 }, 50, 20);
        painter.fillCircle(Color::Magenta, pos + Vec2{ 370, 100 }, 50, 20);

        painter.drawLine(Color::PaleRed, pos + Vec2{ 330, 150 }, pos + Vec2{ 350, 600 }, 8);

        std::vector<Vec2> points{ pos+Vec2{100,150}, pos+Vec2{150,240,},pos+Vec2{200,150},pos+Vec2{300, 240} };
        painter.drawLines(Color::PaleRed, points.data(), points.size(), true, 2.0f);

        painter.drawRect(Color::LightBlue, Rect{ pos + Vec2{100, 250}, Vec2{200,100} });
        painter.drawRect(Color::LightBlue, Rect{ pos + Vec2{100, 360}, Vec2{200,100} }, 10, 10);
        painter.fillRect(Color::Red, Rect{ pos + Vec2{100, 480}, Vec2{200,100} }, 10);

        painter.drawTriangle(Color::DarkRed, pos + Vec2{ 100, 610 }, pos + Vec2{ 100, 650 }, pos + Vec2{ 200,610 }, 3.0f);
        painter.fillTriangle(Color::DarkRed, pos + Vec2{ 210, 610 }, pos + Vec2{ 310, 610 }, pos + Vec2{ 210,750 });

        {
            auto& resourceMgr = application()->resourceManager();
            auto font = resourceMgr.fontManager().get("fonts/msyh.ttf"_hs, 30);

            painter.drawText("这就是 ImFont 字体，Can you See?", font, pos + Vec2{ 100, -50 }, Color::Pink);
            painter.drawText("Where is the sages of passed? 后不见来者, The sky and earth forever last, 独怆然而涕下。",
                                font, pos + Vec2{ 100, 0 }, Color::DarkCyan, 250.0f);

            auto tiger = resourceMgr.textureManager().get("textures/UI/tiger.svg"_hs);
            Rect src = Rect{ Vec2{0.0f, 0.0f }, tiger->size() };
            Rect dst = Rect{ pos + Vec2{0, 700}, tiger->size() / 3.0f };
            painter.drawTexture(tiger, src, dst);

            src = Rect{ 83.0f, 163.0f, 74.0f, 74.0f };
            dst = Rect{ pos + Vec2{200, 750}, Vec2{148, 147} };
            auto etc = resourceMgr.textureManager().get("textures/battle_of_tiles/etc.png"_hs);
            painter.drawTexture(etc, src, dst, 20.0f, Color::Green);
        }

        float dis = 200.0f;
        Vec2 center = pos + Vec2{-300, 300};
        for (float r = 0; r < 2 * PI; r += 0.5f)
        {
            Vec2 dst = center + Vec2 { sin(r)*dis, cos(r)*dis };
            painter.drawLightning(Color::White, center, dst, 100);
        }

        for (const auto& data : _lightning)
        {
            painter.drawLightningData(Color::PaleBlue, {0,0}, data, 2.0f);
        }

        auto t1 = textureMgr.getTexTile("1-0", "role");
        painter.drawTexTile(t1, Rect{ pos + Vec2{ 600, 100 },  {100, 100} });

        auto t5 = textureMgr.getTexTile("5-0", "role");
        painter.drawTexTile(t5, Rect{ pos + Vec2{ 600, 300 },  {150, 150} });

        auto t11 = textureMgr.getTexTile("house1g", "scene");
        painter.drawTexTile(t11, Rect{ pos + Vec2{ 600, 500 },  {150, 150} });

        auto tsk = textureMgr.getTexTile("textures/sticker/skeleton.png");
        painter.drawTexTile(tsk, Rect{ pos + Vec2{ 600, 700 },  tsk->rect().size()});

    }

    void SamplePluginDraw::paint()
    {
#if 0
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
#endif
    }

    void SamplePluginDraw::gfx_paint()
    {
#if 0
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
        auto font = application()->resourceManager().fontManager().get("fonts/Vonwaon.ttf"_hs, 20);
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
#endif
    }
}