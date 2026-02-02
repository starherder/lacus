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
    
    void SamplePluginQuadTree::onInit()  
    {
        application()->eventDispatcher().onMouseMotion.connect(this, &SamplePluginQuadTree::onMouseMotion, -1);
        application()->eventDispatcher().onMouseLeftClicked.connect(this, &SamplePluginQuadTree::onMouseLeftClick, -1);

        auto scenebox = BoxType(0.0f, 0.0f, _worldSize.x, _worldSize.y);

        auto getAABB = [](Object* node)
        {
            return node->getBox();
        };

        _quadtree = std::make_unique<QuadTreeType>(scenebox, getAABB);

        for (int i = 0; i < ObjectCount; i++)
        {
            addRandomObject(i);
        }
    }

    void SamplePluginQuadTree::addRandomObject(int id)
    {
        auto obj = std::make_shared<Object>();
        obj->id = id;
        obj->size = { rand() % ObjectWidth, rand() % ObjectHeight };
        obj->pos = { rand() % (int)(_worldSize.x - ObjectWidth), rand() % (int)(_worldSize.y - ObjectHeight) };
        obj->color.fromHSV({ (float)(30 + rand() % 300), 1.0f, 1.0f, 1.0f });

        _objects.insert({ id, obj });

        _quadtree->add(obj.get());
    }

    void SamplePluginQuadTree::addObjectAtPos(int id, const Vec2& pos)
    {
        static const Vec2 ObjSize = { 20, 20 };

        auto obj = std::make_shared<Object>();
        obj->id = id;
        obj->size = ObjSize;
        obj->pos = { pos.x-ObjSize.x/2, pos.y-ObjSize.y/2 };
        obj->color = Color::Cyan;

        _objects.insert({ id, obj });
        _quadtree->add(obj.get());
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
    };

    void SamplePluginQuadTree::onDraw()  
    {
        auto drawScene = [this]()
        {
            ImGui::GetBackgroundDrawList()->AddRect({ _worldPos.x, _worldPos.y },
                { _worldPos.x + _worldSize.x, _worldPos.y + _worldSize.y }, toImColor(Color::Green), 0, 0, 3.0f);
        };

        auto drawObject = [this](Object* obj)
        {
            float thickness = obj->select ? 5.0f : 1.0f;

            ImGui::GetBackgroundDrawList()->AddRect({ obj->pos.x + _worldPos.x, obj->pos.y + _worldPos.y },
                { obj->pos.x + obj->size.x + _worldPos.x, obj->pos.y + obj->size.y + _worldPos.y }, toImColor(obj->color), 0, 0, thickness);
        };

        drawScene();

        for (auto& [id, obj] : _objects)
        {
            drawObject(obj.get());
        }

        drawQuadNode(_quadtree->getRoot());
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

        for (auto& [id, obj] : _objects)
        {
            obj->select = false;
        }

        if (_opMode == OperatorMode::OP_Add)
        {
            addObjectAtPos((int)_quadtree->count(), scenePos);
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

    void SamplePluginQuadTree::onMouseMotion(const Vec2& pos, const Vec2& offset)
    {
        auto scenePos = pos - _worldPos;
        if (scenePos.x < 0 || scenePos.y < 0 || scenePos.x>_worldSize.x - 1 || scenePos.y>_worldSize.y - 1)
        {
            return;
        }

        for (auto& [id, obj] : _objects)
        {
            obj->select = false;
        }


        auto objects = _quadtree->query({ scenePos.x, scenePos.y, 1,1 });
        for (auto& obj : objects)
        {
            obj->select = true;
        }
    }
}