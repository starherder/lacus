#include "sample_plugin_quadtree.h"


namespace samples 
{

    ImFormQuadTree::ImFormQuadTree(engine::Application* app, SamplePluginQuadTree* plugin) 
        : _app(app), _plugin(plugin)
    {
    }
    
    ImFormQuadTree::~ImFormQuadTree()
    {
    }

	void ImFormQuadTree::onInit()
	{
	}

    void ImFormQuadTree::onClose()
    {
    }

	void ImFormQuadTree::draw()
	{
        ImGui::Begin("quadtree");
        {
            ImGui::Text("objects: %d", _plugin->objectCount());

            ImGui::Separator();

            if (ImGui::RadioButton("contain", _queryMode == quadtree::QueryMode::Contain))
            {
                _queryMode = quadtree::QueryMode::Contain;
                _plugin->setQueryMode(_queryMode);
            }

            ImGui::SameLine();

            if (ImGui::RadioButton("contain center", _queryMode == quadtree::QueryMode::ContainCenter))
            {
                _queryMode = quadtree::QueryMode::ContainCenter;
                _plugin->setQueryMode(_queryMode);
            }

            ImGui::SameLine();

            if (ImGui::RadioButton("intersect", _queryMode == quadtree::QueryMode::Intersect))
            {
                _queryMode = quadtree::QueryMode::Intersect;
                _plugin->setQueryMode(_queryMode);
            }

            ImGui::Separator();

            if (ImGui::RadioButton("add", _opMode == OperatorMode::OP_Add))
            {
                _opMode = OperatorMode::OP_Add;
                _plugin->setOperatorMode(_opMode);
            }

            if (ImGui::RadioButton("del", _opMode == OperatorMode::OP_Del))
            {
                _opMode = OperatorMode::OP_Del;
                _plugin->setOperatorMode(_opMode);
            }

            if (ImGui::RadioButton("select", _opMode == OperatorMode::OP_Select))
            {
                _opMode = OperatorMode::OP_Select;
                _plugin->setOperatorMode(_opMode);
            }

            if (ImGui::RadioButton("select_in_rect", _opMode == OperatorMode::OP_SelectInRect))
            {
                _opMode = OperatorMode::OP_SelectInRect;
                _plugin->setOperatorMode(_opMode);
            }

            if (ImGui::RadioButton("select_in_circle", _opMode == OperatorMode::OP_SelectInCircle))
            {
                _opMode = OperatorMode::OP_SelectInCircle;
                _plugin->setOperatorMode(_opMode);
            }

            ImGui::Separator();

            static bool s_motion = false;
            if (ImGui::Checkbox("motion", &s_motion))
            {
                _plugin->setMotion(s_motion);
            }

            ImGui::SameLine();

            static bool s_add_del = false;
            if (ImGui::Checkbox("+/-", &s_add_del))
            {
                _plugin->setAutoAddDel(s_add_del);
            }


        }
        ImGui::End();
	}

    ///////////////////////////////////////////////////////////////////////

    static ImColor toImColor(const Color& color)
    {
        return ImColor{ color.r, color.g, color.b, color.a };
    }

    SamplePluginQuadTree::SamplePluginQuadTree()
    {
    }

    SamplePluginQuadTree::~SamplePluginQuadTree()
    {
    }

    void SamplePluginQuadTree::setQueryMode(quadtree::QueryMode mode)
    {
        _quadtree->setQueryMode(mode);
    }

    void SamplePluginQuadTree::onInit()  
    {
        application()->eventDispatcher().onMouseMotion.connect(this, &SamplePluginQuadTree::onMouseMotion, -1);
        application()->eventDispatcher().onMouseLeftClicked.connect(this, &SamplePluginQuadTree::onMouseLeftClick, -1);
        application()->eventDispatcher().onMouseLeftDown.connect(this, &SamplePluginQuadTree::onMouseLeftDown, -1);
        application()->eventDispatcher().onMouseLeftUp.connect(this, &SamplePluginQuadTree::onMouseLeftUp, -1);
        application()->eventDispatcher().onMouseLeftDrag.connect(this, &SamplePluginQuadTree::onMouseLeftDrag, -1);

        auto scenebox = BoxType(0.0f, 0.0f, _worldSize.x, _worldSize.y);

        auto getAABB = [](Object* node)
        {
            return node->getBox();
        };

        _quadtree = std::make_unique<QuadTreeType>(scenebox, getAABB);

        for (int i = 0; i < ObjectCount; i++)
        {
            addRandomObject(_randIndex++);
        }
    }

    void SamplePluginQuadTree::addRandomObject(int id)
    {
        auto obj = std::make_shared<Object>();
        obj->id = id;
        obj->size = { rand() % ObjectWidth, rand() % ObjectHeight };
        obj->pos = { rand() % (int)(_worldSize.x - ObjectWidth), rand() % (int)(_worldSize.y - ObjectHeight) };
        obj->color.fromHSV({ (float)(30 + rand() % 300), 1.0f, 1.0f, 1.0f });
        obj->vel = { utility::RandomHelper::randomReal(-ObjectSpeed, ObjectSpeed), utility::RandomHelper::randomReal(-ObjectSpeed, ObjectSpeed) };

        if (_quadtree->getBox().contains(obj->getBox()))
        {
            _quadtree->add(obj.get());
            _objects.insert({ id, obj });
        }
    }

    void SamplePluginQuadTree::addObjectAtPos(int id, const Vec2& pos)
    {
        static const Vec2 ObjSize = { 20, 20 };

        auto obj = std::make_shared<Object>();
        obj->id = id;
        obj->size = ObjSize;
        obj->pos = { pos.x-ObjSize.x/2, pos.y-ObjSize.y/2 };
        obj->color = Color::Cyan;

        if (_quadtree->getBox().contains(obj->getBox()))
        {
            _quadtree->add(obj.get());
            _objects.insert({ id, obj });
        }
    }

    void SamplePluginQuadTree::removeObject(Object* obj)
    {
        if (!obj) return;

        _quadtree->remove(obj);
        _objects.erase(obj->id);
    }

    void SamplePluginQuadTree::onInstall()  
    {
    }

    void SamplePluginQuadTree::onUninstall()  
    {
    }

    void SamplePluginQuadTree::onEnable() 
    {
        imgui::ImFormManager::inst().showForm<ImFormQuadTree>("ImFormQuadTree", application(), this);
    }

    void SamplePluginQuadTree::onDisable() 
    {
        imgui::ImFormManager::inst().closeForm("ImFormQuadTree");
    }

    void SamplePluginQuadTree::onUpdate()  
    {
        const int ChangePerSecond = 10;
        float ts = application()->frameTicker().deltaSeconds();

        static int passed_ms = 0;
        passed_ms += application()->frameTicker().deltaTicks();
        if(passed_ms > 10)
        {
            if(_addDel)
            {
                removeObject(_objects.begin()->second.get());
                addRandomObject(_randIndex++);
            }
            passed_ms = 0;
        }

        if (_motion)
        {
            for (auto& [id, obj] : _objects)
            {
                auto pos = obj->pos + obj->vel * ts;
                if (!_quadtree->getBox().contains({ pos.x, pos.y, obj->size.x, obj->size.y }))
                {
                    int flag = (rand()%2) == 0 ? -1 : 1;
                    obj->vel.x *= flag;
                    obj->vel.y *= (-flag);
                }
                else
                {
                    obj->pos = pos;
                }
            }
        }

        _quadtree->clear();

        for (auto& [id, obj] : _objects)
        {
            _quadtree->add(obj.get());
        }
    }

    void SamplePluginQuadTree::drawQuadNode(QuadTreeType::Node* node)
    {
        if (!node) return;

        Vec2 pos1 = { node->box.left + _worldPos.x, node->box.top + _worldPos.y };
        Vec2 pos2 = { pos1.x + node->box.width, pos1.y + node->box.height };
        ImGui::GetBackgroundDrawList()->AddRect({ pos1.x, pos1.y }, { pos2.x, pos2.y }, toImColor(Color::White), 0, 0, 2.0f);

        for (auto& child : node->children)
        {
            drawQuadNode(child.get());
        }
    }

    void SamplePluginQuadTree::drawSelectGizmo()
    {
        if (_opMode == OP_SelectInCircle)
        {
            auto center = _selectRange.pos() + _selectRange.size() / 2.0f + _worldPos;
            auto radius = glm::length(_selectRange.size()) / 2.0f;
            ImGui::GetBackgroundDrawList()->AddCircle({ center.x, center.y }, radius, toImColor(Color::Red), 30);
        }
        else if (_opMode == OP_SelectInRect)
        {
            auto lt = _selectRange.pos() + _worldPos;
            auto rb = _selectRange.pos() + _selectRange.size() + _worldPos;
            ImGui::GetBackgroundDrawList()->AddRect({ lt.x, lt.y }, { rb.x, rb.y }, toImColor(Color::Red), 0, 0, 2.0f);
        }
    }

    void SamplePluginQuadTree::onDraw()  
    {
        auto drawScene = [this]()
        {
            ImGui::GetBackgroundDrawList()->AddRect({ _worldPos.x, _worldPos.y },
                { _worldPos.x + _worldSize.x, _worldPos.y + _worldSize.y }, toImColor(Color::Green), 0, 0, 3.0f);
        };

        auto drawObject = [this](Object* obj)
        {
            if (obj->select)
            {
                ImGui::GetBackgroundDrawList()->AddRectFilled({ obj->pos.x + _worldPos.x, obj->pos.y + _worldPos.y },
                    { obj->pos.x + obj->size.x + _worldPos.x, obj->pos.y + obj->size.y + _worldPos.y }, toImColor(obj->color));
            }
            else
            {
                ImGui::GetBackgroundDrawList()->AddRect({ obj->pos.x + _worldPos.x, obj->pos.y + _worldPos.y },
                    { obj->pos.x + obj->size.x + _worldPos.x, obj->pos.y + obj->size.y + _worldPos.y }, toImColor(obj->color), 0, 0, 1.0f);
            }
        };

        drawScene();

        for (auto& [id, obj] : _objects)
        {
            drawObject(obj.get());
        }

        drawQuadNode(_quadtree->getRoot());

        drawSelectGizmo();
    }

    void SamplePluginQuadTree::onClose()  
    {
    }
    
    void SamplePluginQuadTree::onMouseLeftClick(const Vec2& pos)
    {
        auto scenePos = pos - _worldPos;
        if (scenePos.x < 0 || scenePos.y < 0 || scenePos.x>_worldSize.x - 1 || scenePos.y>_worldSize.y - 1)
        {
            return;
        }

        if (_opMode == OperatorMode::OP_Select)
        {
            unselectAll();

            auto objects = _quadtree->query({ scenePos.x, scenePos.y, 1,1 });
            for (auto& obj : objects)
            {
                obj->select = true;
            }
        }

        if (_opMode == OperatorMode::OP_Add)
        {
            addObjectAtPos(_randIndex++, scenePos);
        }

        if (_opMode == OperatorMode::OP_Del)
        {
            auto objects = _quadtree->query({ scenePos.x, scenePos.y, 1,1 });
            for (auto& obj : objects)
            {
                removeObject(obj);
            }
        }
    }

    void SamplePluginQuadTree::onMouseLeftDown(const Vec2& pos)
    {
        if (_opMode == OP_SelectInCircle || _opMode == OP_SelectInRect)
        {
            unselectAll();

            auto beginPos = pos - _worldPos;
            _selectRange.x = beginPos.x;
            _selectRange.y = beginPos.y;
            _selectRange.w = 0;
            _selectRange.h = 0;
        }
    }
    
    void SamplePluginQuadTree::onMouseLeftUp(const Vec2& pos)
    {
        _selectRange = {0, 0, 0, 0};
    }

    void SamplePluginQuadTree::onMouseLeftDrag(const Vec2& pos, const Vec2& offset)
    {
        if (_opMode != OP_SelectInCircle && _opMode != OP_SelectInRect)
        {
            return;
        }
        
        unselectAll();

        auto endPos = pos - _worldPos;
        _selectRange.w = endPos.x - _selectRange.x;
        _selectRange.h = endPos.y - _selectRange.y;

        if (_opMode == OP_SelectInCircle)
        {
            quadtree::Vector2 center = { _selectRange.x + _selectRange.w / 2, _selectRange.y + _selectRange.h / 2 };
            auto radius = glm::length(_selectRange.size() ) / 2.0f;

            quadtree::Circle<float> circle{center, radius};
            auto objects = _quadtree->query(circle);
            for (auto& obj : objects)
            {
                obj->select = true;
            }
        }

        if (_opMode == OP_SelectInRect)
        {
            Rect range = _selectRange;
            if (range.w < 0) {
                range.x += range.w;
                range.w *= -1;
            }
            if (range.h < 0) {
                range.y += range.h;
                range.h *= -1;
            }

            quadtree::Box<float> rect{range.x, range.y, range.w, range.h};

            auto objects = _quadtree->query(rect);
            for (auto& obj : objects)
            {
                obj->select = true;
            }
        }
    }

    void SamplePluginQuadTree::unselectAll()
    {
        for (auto& [id, obj] : _objects)
        {
            obj->select = false;
        }
    }

    void SamplePluginQuadTree::onMouseMotion(const Vec2& pos, const Vec2& offset)
    {
    }
}