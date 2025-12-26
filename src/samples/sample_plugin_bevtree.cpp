#include "sample_plugin_bevtree.h"

#include "engine/application.h"

#include "braintree/brain_tree.h"
#include <string>

namespace samples {

    const Vec2 NpcDefaultSize = { 32, 32 };

    Vec2i g_wndSize = { 1280, 1024 };
    float g_frameSeconds = 0.0f;

    class NPC : public Actor
    {
    public:
        NPC() = default;
        ~NPC() = default;
        NPC(const std::string& name)
            : _name(name) {}

        void setBevTree(BrainTree::Node::Ptr bevTree)
        {
            _bevTree = bevTree;
        }

        void update() override
        {
            _bevTree->update();
        }

        void draw(Renderer& renderer)
        {
            renderer.setDrawColor(color);
            renderer.drawFillRect({ (pos - size / 2.0f), size });

            renderer.setDrawColor(Color::Gray);
            renderer.drawDebugText(pos, _name.c_str());
        }

        Vec2 pos = { 0, 0 };
        Vec2 size = NpcDefaultSize;
        Vec2 vel = { 0, 0 };
        Color color = Color::Red;
        float speed = 1.0f;

    private:
        BrainTree::Node::Ptr _bevTree = nullptr;
        std::string _name;
    };

    // =================================================================

    class BtNode : public BrainTree::Node
    {
    public:
        BtNode() = delete;
        ~BtNode() = default;
        BtNode(const std::string& name) : _name(name) {}
    protected:
        std::string _name;
    };


    // ---------------------------------------------------------
    class Move : public BtNode
    {
    public:
        Move() = delete;
        ~Move() = default;
        Move(const std::string& name, NPC* npc)
            : BtNode(name), _npc(npc)
        { }

        void initialize() override
        {
            spdlog::info("Move({}) initialize", _name);

            int symx = (rand() % 2 == 0) ? 1 : -1;
            int symy = (rand() % 2 == 0) ? 1 : -1;
            int lenx = symx * (rand() % 300);
            int leny = symy * rand() % 300;

            _target = _npc->pos + Vec2(lenx, leny);
            _npc->vel = glm::normalize(_target - _npc->pos) * _npc->speed;
        }

        void terminate(Status s) override
        {
            spdlog::info("Move({}) terminate", _name);
        }

        Status update() override
        {
            _npc->pos += _npc->vel * g_frameSeconds;

            if (glm::length(_npc->pos - _target) < 10.0f) {
                _npc->pos = _target;
                return Node::Status::Success;
            }

            if (_npc->pos.x < 50 || _npc->pos.x > g_wndSize.x - 50
                || _npc->pos.y < 50 || _npc->pos.y > g_wndSize.y - 50)
            {
                return Node::Status::Failure;
            }

            return Node::Status::Running;
        }

    private:
        NPC* _npc = nullptr;
        Vec2 _target;
    };


    // ---------------------------------------------------------
    class Resize : public BtNode
    {
        enum { ScaleNone, ScaleUp, ScaleDown, ScaleFinish };
    public:
        Resize() = delete;
        ~Resize() = default;
        Resize(const std::string& name, NPC* npc)
            : BtNode(name), _npc(npc)
        { }

        void initialize() override
        {
            spdlog::info("Resize({}) initialize", _name);
            _npc->size = NpcDefaultSize;
            _scaleMode = ScaleNone;
        }

        void terminate(Status s) override
        {
            spdlog::info("Resize({}) terminate", _name);
            _npc->size = NpcDefaultSize;
            _scaleMode = ScaleNone;
        }

        Status update() override
        {
            if (_scaleMode == ScaleNone) {
                _scaleMode = ScaleUp;
            }
            else if (_scaleMode == ScaleUp) {
                _npc->size += _vec *g_frameSeconds;
                if (glm::distance(_npc->size, _target) < 5.0f) {
                    _npc->size = _target;
                    _scaleMode = ScaleDown;
                }
            }
            else if (_scaleMode == ScaleDown) {
                _npc->size -= _vec *g_frameSeconds;
                if (glm::distance(_npc->size, NpcDefaultSize) < 5.0f) {
                    _npc->size = NpcDefaultSize;
                    _scaleMode = ScaleFinish;
                }
            }
            else {
                return Node::Status::Success;
            }

            return Node::Status::Running;
        }

    private:
        NPC* _npc = nullptr;
        Vec2 _target = NpcDefaultSize * 3.0f;
        int _scaleMode = ScaleNone;
        Vec2 _vec = {100, 100};
    };


    // ---------------------------------------------------------
    class Alpha : public BtNode
    {
    public:
        Alpha() = delete;
        ~Alpha() = default;
        Alpha(const std::string& name, NPC* npc)
            : BtNode(name), _npc(npc)
        { }

        void initialize() override
        {
            spdlog::info("Alpha({}) initialize", _name);
        }

        void terminate(Status s) override
        {
            _npc->color.a = 255;
            spdlog::info("Alpha({}) terminate", _name);
        }

        Status update() override
        {
            if (_npc->color.a > 0)
            {
                _npc->color.a -= 1;
                return Node::Status::Running;
            }
            else
            {
                return Node::Status::Success;
            }
        }

    private:
        NPC* _npc = nullptr;

    };

    // ---------------------------------------------------------
    class Hue : public BtNode
    {
    public:
        Hue() = delete;
        ~Hue() = default;
        Hue(const std::string& name, NPC* npc)
            : BtNode(name), _npc(npc)
        { }

        void initialize() override
        {
            spdlog::info("Alpha({}) initialize", _name);
            _srcColor = _npc->color;

            _target = _npc->color.toHSV().h + 90;
            _target = (_target > 360) ? (360 - _target) : _target;
        }

        void terminate(Status s) override
        {
            _npc->color = _srcColor;
            spdlog::info("Alpha({}) terminate", _name);
        }

        Status update() override
        {
            auto hsv = _npc->color.toHSV();
            if (std::fabs(hsv.h - _target) > 0.3)
            {
                hsv.h += (_target - hsv.h) > 0 ? 0.3 : -0.3;
                Color c; c.fromHSV(hsv);
                _npc->color = c;
                return Node::Status::Running;
            }
            else
            {
                return Node::Status::Success;
            }
        }

    private:
        NPC* _npc = nullptr;
        float _target = 0;
        Color _srcColor;

    };

    // =============================================================

    ///////////////////////////////////////////////////////////////////////////////////////
    void SamplePluginBevTree::onInit()
    {
        g_wndSize = application()->window().getSize();

        auto npc0 = std::make_shared<NPC>("wang");
        auto bev_0 = BrainTree::Builder()
            .composite<BrainTree::Sequence>() 
                .decorator<BrainTree::Repeater>(0) 
                    .leaf<Move>("Move1", npc0.get())
                .end()
            .end()
            .build();

        npc0->setBevTree(bev_0);
        npc0->pos = { 700, 350 };
        npc0->color = Color::Yellow;
        npc0->speed = 200.0f;
        _actors.push_back(npc0);

        auto npc1 = std::make_shared<NPC>("zhang");
        auto bev_1 = BrainTree::Builder()
            .composite<BrainTree::Sequence>() 
                .decorator<BrainTree::Repeater>(3) // 执行三次
                    .leaf<Move>("Move1", npc1.get())
                .end()
                .leaf<Resize>("Resize1", npc1.get())
                .leaf<Alpha>("Alpha1", npc1.get())
                .leaf<Hue>("Hue1", npc1.get())
            .end()
            .build();

        npc1->setBevTree(bev_1);
        npc1->pos = { 200, 150 };
        npc1->speed = 150.0f;
        _actors.push_back(npc1);


        auto npc2 = std::make_shared<NPC>("li");
        auto bev_2 = BrainTree::Builder()
            .composite<BrainTree::Sequence>()
            .leaf<Move>("Move2", npc2.get())
            .leaf<Resize>("Resize2", npc2.get())
            .leaf<Alpha>("Alpha2", npc2.get())
            .leaf<Hue>("Hue2", npc2.get())
            .end().build();

        npc2->setBevTree(bev_2);
        npc2->color = Color::Cyan;
        npc2->pos = { 500, 400 };
        npc2->speed = 200.0f;
        _actors.push_back(npc2);
    }

    void SamplePluginBevTree::onEnable()
    {
    }

    void SamplePluginBevTree::onDisable()
    {
    }

    void SamplePluginBevTree::onUpdate()
    {
        static auto old_ticks = SDL_GetTicks();
        auto cur_ticks = SDL_GetTicks();

        g_frameSeconds = (float)(cur_ticks - old_ticks) / 1000.0f;
        old_ticks = cur_ticks;

        for (auto& actor : _actors)
        {
            actor->update();
        }
    }

    void SamplePluginBevTree::onDraw()
    {
        for (auto& actor : _actors)
        {
            actor->draw(application()->renderer());
        }
    }

}