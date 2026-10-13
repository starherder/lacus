#include "sample_plugin_draw.h"

#include "engine/application.h"
#include "engine/color.h"
#include "engine/animation.h"

#include "imform/imform_manager.h"

namespace samples {

    void ImFormTexSet::onInit()
    {
        auto& textureSets = _application->resourceManager().textureManager().getTexSets();
        for (auto& [setname, texset] : textureSets)
        {
            _texSets.push_back(setname.c_str());
        }
    }

    void ImFormTexSet::draw()
    {
        int PickerFlag = ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel;

        ImGui::Begin("textures");
            static bool showBorder = false;
            ImGui::Checkbox("border##texset", &showBorder);

            ImGui::SameLine();

            static float borderColor[4] = { 1.0f, 0.0f, 0.0f, 1.0f };;
            ImGui::ColorEdit4("##texset_border_color", borderColor, PickerFlag);
  
            static bool showGround = false;
            ImGui::Checkbox("ground##texset", &showGround);

            ImGui::SameLine();

            static float groundColor[4] = { 0.5f, 0.5f, 0.5f, 1.0f };
            ImGui::ColorEdit4("##texset_ground_color", groundColor, PickerFlag);

            static int texset_select_index = 0;
            if (ImGui::ListBox("tex_sets##texset_list", &texset_select_index, _texSets.data(), (int)_texSets.size()))
            {
                _selectTexSet = _texSets[texset_select_index];

                auto& textureSets = _application->resourceManager().textureManager().getTexSets();
                auto it = textureSets.find(_selectTexSet);
                if (it == textureSets.end())
                {
                    return;
                }

                _texTiles.clear();

                auto& texset = it->second;
                for (auto& [name, textile] : texset.tileset)
                {
                    _texTiles.push_back(name.c_str());
                }
            }

            static int textile_select_index = 0;
            if (ImGui::ListBox("tex_tiles##textile_list", &textile_select_index, _texTiles.data(), (int)_texTiles.size()))
            {
                auto selectTile = _texTiles[textile_select_index];
                _selectTile = _application->resourceManager().textureManager().getTexTile(selectTile, _selectTexSet);
            }

            // color
            static float blend_color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
            ImGui::ColorEdit4("blend_color##texset_ground_color", blend_color);

            // scale
            if (ImGui::Button("reset##texset_scale")) {
                _scale = 1.0f;
            }
            ImGui::SameLine();
            ImGui::SliderFloat("scale##texset", &_scale, 0.1f, 10.0f);

            // rotate
            if (ImGui::Button("reset##texset_rotate")) {
                _rotate = 0.0f;
            }
            ImGui::SameLine();
            ImGui::SliderFloat("rotate##texset", &_rotate, -PI * 2, PI * 2);

            // round
            if (ImGui::Button("reset##texset_round")) {
                _round = 0.0f;
            }
            ImGui::SameLine();
            ImGui::SliderFloat("round##texset", &_round, 0.0f, 100.0f);

            static bool showRotate = false;
            ImGui::Checkbox("test_rotate", &showRotate);
            
            if (showRotate) 
            {
                testRotate();
            }

        ImGui::End();

        auto& painter = _application->painter();

        if (_selectTile)
        {
            Rect rect = Rect{ 100.0f, 100.0f, _selectTile->rect().w * _scale, _selectTile->rect().h * _scale };

            if (showGround)
            {
                Color gc { groundColor[0], groundColor[1], groundColor[2], groundColor[3] };
                painter.fillRect(gc, rect);
            }

            Color bc { blend_color[0], blend_color[1], blend_color[2], blend_color[3] };
            painter.drawTexTile(_selectTile, rect, _rotate, _round, bc);

            if (showBorder)
            {
                Color bc { borderColor[0], borderColor[1], borderColor[2], borderColor[3] };
                painter.drawRect(bc, rect);
            }
        }
    }


    void ImFormTexSet::testRotate()
    {
        auto& painter = _application->painter();

        if(_selectTile)
        {
            Vec2 srcPos = { 700, 600 };
            //Vec2 dstPos = { 500, 500 };

            static float rot = 0.0f;
            rot += 1.0f * _application->frameTicker().deltaSeconds();
            rot = rot > PI * 2 ? 0 : rot;

            Vec2 dstPos = Geometry::RotatePoint({ 500, 500 }, srcPos, rot);

            Vec2 diff = dstPos - srcPos;

            Vec2 srcSize = _selectTile->rect().size();

            Vec2 offset = srcSize + _selectTile->center();
            Vec2 fixPos = srcPos - offset;

            float radian = glm::orientedAngle(Vec2{1.0f, 0.0f}, glm::normalize(diff))  + glm::radians(-90.0f);
            float degree = glm::degrees(radian);

            painter.drawTexTile(_selectTile, { fixPos, srcSize }, radian, 0.0f, Color::White);

            painter.fillCircle(Color::Red, srcPos, 10);
            painter.fillCircle(Color::Green, dstPos, 10);
            painter.drawLine(Color::Yellow, srcPos, dstPos);
        }
    }

    // ============================================================================

    void ImFormAnimSet::onInit()
    {
        auto& animMgr = _application->resourceManager().animationManager();
        _animNameStore = animMgr.getAllNames();
        for (auto& name : _animNameStore)
        {
            _animNames.push_back(name.c_str());
        }
    }

    void ImFormAnimSet::draw()
    {
        ImGui::Begin("animations");

        static int selectIndex = 0;
        if (ImGui::ListBox("anims##animset_list", &selectIndex, _animNames.data(), (int)_animNames.size()))
        {
            _selectAnimName = _animNames[selectIndex];
            _selectAnim = _application->resourceManager().animationManager().create(
                _selectAnimName, _selectAnimName);
            if (_selectAnim)
            {
                _selectAnim->play();
            }
        }

        if (_selectAnim)
        {
            ImGui::Separator();
            ImGui::Text("name: %s", _selectAnimName.c_str());
            ImGui::Text("frames: %d", _selectAnim->frameCount());

            int curFrame = _selectAnim->getCurrentFrameIndex();
            ImGui::Text("current frame: %d", curFrame);

            // 如果当前帧有事件，显示事件名
            if (curFrame >= 0 && curFrame < _selectAnim->frameCount())
            {
                const auto& frame = _selectAnim->getFrame(curFrame);
                if (!frame.event.empty())
                {
                    ImGui::Text("event: %s", frame.event.c_str());
                }
            }

            // loop
            bool loop = _selectAnim->loop();
            if (ImGui::Checkbox("loop", &loop))
            {
                _selectAnim->setLoop(loop);
            }

            // rate
            float rate = _selectAnim->rate();
            if (ImGui::SliderFloat("rate", &rate, 0.1f, 10.0f))
            {
                _selectAnim->setRate(rate);
            }

            // playback buttons
            if (_selectAnim->isPlaying())
            {
                if (ImGui::Button("pause"))
                {
                    _selectAnim->stop();
                }
            }
            else
            {
                if (ImGui::Button("play"))
                {
                    _selectAnim->play();
                }
            }

            ImGui::SameLine();
            if (ImGui::Button("reset"))
            {
                _selectAnim->reset();
                _selectAnim->play();
            }

            // update animation
            _selectAnim->update(_application->frameTicker().deltaSeconds());

            // draw preview
            auto tex = _selectAnim->getCurrentTexture();
            if (tex)
            {
                float previewSize = 150.0f;
                Rect dstRect = {Vec2{50.0f, 50.0f}, Vec2{previewSize, previewSize}};
                _selectAnim->draw(_application->painter(), dstRect);
            }
        }

        ImGui::End();
    }

    // ============================================================================


    void ImGuiFormDraw::onInit()
    {
    }

    void ImGuiFormDraw::draw()
    {
        ImGui::Begin("drawlist");
        {
            PainterDrawMode mode = _plugin->drawMode();

            if (ImGui::RadioButton("sdl_draw", mode == PainterDrawMode::Sdl))
            {
                _plugin->setDrawMode(PainterDrawMode::Sdl);
            }
            ImGui::SameLine();
            if (ImGui::RadioButton("paint_draw", mode == PainterDrawMode::Paint))
            {
                _plugin->setDrawMode(PainterDrawMode::Paint);
            }
            ImGui::SameLine();
            if (ImGui::RadioButton("gfx_draw", mode == PainterDrawMode::Gfx))
            {
                _plugin->setDrawMode(PainterDrawMode::Gfx);
            }
            ImGui::SameLine();
            if (ImGui::RadioButton("im_draw", mode == PainterDrawMode::Im))
            {
                _plugin->setDrawMode(PainterDrawMode::Im);
            }
        }
        ImGui::End();
    }

    // ============================================================================

    void SamplePluginDraw::onInit() 
    {
        _sdlPainter = std::make_unique<SdlPainter>(*application());
        _paintPainter = std::make_unique<Painter>(*application());
        _gfxPainter = std::make_unique<GfxPainter>(*application());
        _imPainter = std::make_unique<ImPainter>(*application());

        _sdlPainter->init();
        _paintPainter->init();
        _gfxPainter->init();

        auto texturedir = application()->resPath() / "textures";
        application()->resourceManager().textureManager().loadAllTexSets(texturedir);

        initGeometry();
    }

    void SamplePluginDraw::onInstall() 
    {  
    }

    void SamplePluginDraw::onUninstall() 
    {
        if (_gfxPainter)
        {
            _gfxPainter->quit();
        }
        if (_paintPainter)
        {
            _paintPainter->quit();
        }
        if (_sdlPainter)
        {
            _sdlPainter->quit();
        }

        _imPainter.reset();
        _gfxPainter.reset();
        _paintPainter.reset();
        _sdlPainter.reset();
    }

    void SamplePluginDraw::onEnable()
    {
        imgui::ImFormManager::inst().showForm<ImGuiFormDraw>("form_draw", application(), this);
        imgui::ImFormManager::inst().showForm<ImFormTexSet>("form_texset", application(), this);
        imgui::ImFormManager::inst().showForm<ImFormAnimSet>("form_animset", application(), this);
    }

    void SamplePluginDraw::onDisable()
    {
        imgui::ImFormManager::inst().closeForm("form_draw");
        imgui::ImFormManager::inst().closeForm("form_texset");
        imgui::ImFormManager::inst().closeForm("form_animset");
    }

    void SamplePluginDraw::onUpdate() 
    {
    }

    void SamplePluginDraw::onDraw() 
    {
        const Vec2 pos = { 180.0f, 160.0f };

        switch (_drawMode)
        {
        case PainterDrawMode::Sdl:
            drawPainterDemo(*_sdlPainter, pos);
            break;
        case PainterDrawMode::Paint:
            _paintPainter->preFrame();
            drawPainterDemo(*_paintPainter, pos);
            _paintPainter->postFrame();
            break;
        case PainterDrawMode::Gfx:
            drawPainterDemo(*_gfxPainter, pos);
            break;
        case PainterDrawMode::Im:
            drawPainterDemo(*_imPainter, pos);
            break;
        }
    }

    void SamplePluginDraw::onClose() 
    {
        LogInfo("Release sample plugin draw");
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

        _vertexData.vertices.clear();
        _vertexData.indices.clear();

        constexpr int xcount = 30;
        constexpr int ycount = 30;
        constexpr float gridw = 50.0f;
        constexpr float gridh = 50.0f;

        for (int x = 0; x < xcount; x++)
        {
            for (int y = 0; y < ycount; y++)
            {
                auto pos = Vec2f{ x * gridw, y * gridh };
                auto size = Vec2f{ gridw, gridh };

                auto color = Color{ HSVColor{(float)(x % 360), 1.0f, 1.0f, 1.0f } };

                Vertex v1{ {pos.x, pos.y},                   color, {0.0f,      0.0f} };
                Vertex v2{ {pos.x + size.x, pos.y},          color, {0.333333f, 0.0f} };
                Vertex v3{ {pos.x + size.x, pos.y + size.y}, color, {0.333333f, 1.0f} };
                Vertex v4{ {pos.x, pos.y + size.y},          color, {0.0f,      1.0f} };

                _vertexData.vertices.push_back(v1);
                _vertexData.vertices.push_back(v2);
                _vertexData.vertices.push_back(v3);
                _vertexData.vertices.push_back(v4);

                const int vertexOffset = (x * ycount + y) * 4;
                _vertexData.indices.push_back(vertexOffset + 0);
                _vertexData.indices.push_back(vertexOffset + 1);
                _vertexData.indices.push_back(vertexOffset + 2);
                _vertexData.indices.push_back(vertexOffset + 0);
                _vertexData.indices.push_back(vertexOffset + 2);
                _vertexData.indices.push_back(vertexOffset + 3);
            }
        }

    }

    void SamplePluginDraw::drawPainterDemo(IPainter& painter, const Vec2& pos)
    {
        auto& resourceMgr = application()->resourceManager();
        auto& textureMgr = resourceMgr.textureManager();
        auto font = resourceMgr.fontManager().get("fonts/msyh.ttf"_hs, 24);

        if (_texture && !_vertexData.vertices.empty() && !_vertexData.indices.empty())
        {
            painter.drawGeometry(_texture, _vertexData.vertices.data(), (int)_vertexData.vertices.size(),
                _vertexData.indices.data(), (int)_vertexData.indices.size(), pos + Vec2{ 1000.0f, 0.0f }, 0.5f);
        }

        painter.drawText("IPainter render compare", font, pos, Color::Pink);
        painter.drawText("rect / circle / line / triangle / texture / tile / geometry", font, pos + Vec2{ 0, 34 }, Color::DarkCyan);

        painter.drawCircle(Color::Red, pos + Vec2{ 80, 130 }, 45, 32, 6);
        painter.drawCircle(Color::Green, pos + Vec2{ 190, 130 }, 45, 32, 6);
        painter.drawCircle(Color::Blue, pos + Vec2{ 300, 130 }, 45, 32, 6);

        painter.fillCircle(Color::Yellow, pos + Vec2{ 80, 230 }, 45, 32);
        painter.fillCircle(Color::Cyan, pos + Vec2{ 190, 230 }, 45, 32);
        painter.fillCircle(Color::Magenta, pos + Vec2{ 300, 230 }, 45, 32);

        painter.drawLine(Color::PaleRed, pos + Vec2{ 20, 310 }, pos + Vec2{ 360, 350 }, 8);

        std::vector<Vec2> points{ pos + Vec2{ 40, 380 }, pos + Vec2{ 100, 460 }, pos + Vec2{ 170, 380 }, pos + Vec2{ 260, 460 } };
        painter.drawLines(Color::PaleRed, points.data(), points.size(), true, 2.0f);

        painter.drawRect(Color::LightBlue, Rect{ pos + Vec2{ 420, 90 }, Vec2{ 180, 80 } });
        painter.drawRect(Color::LightBlue, Rect{ pos + Vec2{ 420, 190 }, Vec2{ 180, 80 } }, 14, 10);
        painter.fillRect(Color::Red, Rect{ pos + Vec2{ 420, 290 }, Vec2{ 180, 80 } }, 14);

        painter.drawTriangle(Color::DarkRed, pos + Vec2{ 420, 430 }, pos + Vec2{ 420, 500 }, pos + Vec2{ 520, 430 }, 3.0f);
        painter.fillTriangle(Color::DarkRed, pos + Vec2{ 540, 430 }, pos + Vec2{ 640, 430 }, pos + Vec2{ 560, 520 });

        auto tiger = textureMgr.get("textures/UI/tiger.svg"_hs);
        if (tiger)
        {
            Rect src = Rect{ Vec2{ 0.0f, 0.0f }, tiger->size() };
            Rect dst = Rect{ pos + Vec2{ 20, 520 }, tiger->size() / 4.0f };
            painter.drawTexture(tiger, src, dst);
        }

        auto etc = textureMgr.get("textures/bot/etc.png"_hs);
        if (etc)
        {
            Rect src = Rect{ 83.0f, 163.0f, 74.0f, 74.0f };
            Rect dst = Rect{ pos + Vec2{ 220, 540 }, Vec2{ 120, 120 } };
            painter.drawTexture(etc, src, dst, 20.0f, Color::Green);
        }

        auto t1 = textureMgr.getTexTile("1-0", "role");
        if (t1)
        {
            painter.drawTexTile(t1, Rect{ pos + Vec2{ 700, 90 }, { 100, 100 } });
        }

        auto th1 = textureMgr.getTexTile("house1g", "scene");
        if (th1)
        {
            painter.drawTexTile(th1, Rect{ pos + Vec2{ 700, 230 }, { 150, 150 } });
        }

        auto tsk = textureMgr.getTexTile("textures/sticker/skeleton.png");
        if (tsk)
        {
            painter.drawTexTile(tsk, Rect{ pos + Vec2{ 700, 430 }, tsk->rect().size() }, PI * 0.15f);
        }

    }
}
