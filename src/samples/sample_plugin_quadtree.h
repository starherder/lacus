#pragma once

#include "engine/application.h"
#include "imform/imform_manager.h"
#include "imform/imform.h"

#include "quadtree/quad_tree.h"

namespace samples 
{
    using namespace engine;

    using BoxType = quadtree::Box<float>;

    struct Object
    {
        int id;

        bool select = false;
        
        Vec2 pos;
        Vec2 size;

        Color color;

        bool operator == (const Object& obj) 
        {
            return id == obj.id;
        }

        BoxType getBox() const
        {
            return {pos.x, pos.y, size.x, size.y};
        }
    };

    using QuadTreeType = quadtree::Quadtree<Object*, std::function<BoxType(Object*)> >;
    using QuadTreePtr = std::unique_ptr<QuadTreeType>;


    enum OperatorMode
    {
        OP_Add,
        OP_Del,
    };

	class ImFormQuadTree : public imgui::ImForm
	{
	public:
        ImFormQuadTree() = delete;
        ImFormQuadTree(engine::Application* app, class SamplePluginQuadTree* plugin);
        ~ImFormQuadTree();

	protected:
        void onInit() override;
        void onClose() override;

		void draw() override;

	private:
		engine::Application* _app = nullptr;
        SamplePluginQuadTree* _plugin = nullptr;

        OperatorMode _opMode = OperatorMode::OP_Add;
	};


    class SamplePluginQuadTree final : public engine::Plugin , public signals::SlotHandler
    {
    public:
        SamplePluginQuadTree();
        ~SamplePluginQuadTree();

        const char* name() override { return "sample_quadtree_plugin"; }

        void onInit() override ;

        void onInstall() override ;

        void onUninstall() override ;

        void onEnable() override;

        void onDisable() override;

        void onUpdate() override ;

        void onDraw() override ;

        void onClose() override ;

        void onMouseMotion(const Vec2& pos, const Vec2& offset);
        
        void onMouseLeftClick(const Vec2& pos);

        void drawQuadNode(QuadTreeType::Node* node);

        void addRandomObject(int id);

        void addObjectAtPos(int id, const Vec2& pos);

        void removeObject(Object* obj);

        void setOperatorMode(OperatorMode mode) { _opMode = mode; }

    private:
        QuadTreePtr _quadtree = nullptr;

        OperatorMode _opMode = OperatorMode::OP_Add;

        std::map<int, std::shared_ptr<Object>> _objects;

        const int ObjectWidth = 100;
        const int ObjectHeight = 100;

        Vec2 _worldPos = { 50, 50 };
        Vec2 _worldSize = { 2048, 1024 };

        static const int ObjectCount = 100;
    };
}