#include "sample_plugin_bevtree.h"

#include "engine/application.h"

#include "bevtree/brain_tree.h"
#include <string>

namespace samples {

const Vec2 NpcDefaultSize = {32, 32};

float g_frameSeconds = 0.0f;

class NPC  : public Actor
{
public:
    NPC() = default;
    ~NPC() = default;
    NPC(const std::string& name) 
    : _name(name){}

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
        renderer.setDrawColor(Color::Red);
        renderer.drawFillRect({pos, size});
        renderer.drawDebugText(pos, _name.c_str());
    }

    Vec2 pos = {0, 0};
    Vec2 size = NpcDefaultSize;
    Vec2 vel = {0, 0};
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

        int symx = (rand() % 2 == 0)? 1:-1;
        int symy = (rand() % 2 == 0)? 1:-1;
        int lenx = symx*(rand() % 300);
        int leny = symy*rand() % 300;

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

        return Node::Status::Failure;
    }

private:
    NPC* _npc = nullptr;
    Vec2 _target;
};


// ---------------------------------------------------------
class Resize : public BtNode
{
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
    }

    void terminate(Status s) override
    {
        spdlog::info("Resize({}) terminate", _name);
    }

    Status update() override
    {
        if(glm::distance(_npc->size, _target) > 5.0f )
        {
            _npc->size += Vec2{1, 1} *g_frameSeconds;
        }
        else
        {
            _npc->size = _target;
            return Node::Status::Success;
        }
        
        return Node::Status::Failure;
    }

private:
    NPC* _npc = nullptr;
    Vec2 _target = {300, 300};

};

// =============================================================

///////////////////////////////////////////////////////////////////////////////////////
void SamplePluginBevTree::onInit() 
{
    /*
    auto npc1 = std::make_shared<NPC>("zhang");
    auto bev_1 = BrainTree::Builder()
        .composite<BrainTree::Repeater>()
        .leaf<Move>("Move1", npc1.get())
        .leaf<Resize>("Resize1", npc1)
        .end().build();

    npc1->setBevTree(bev_1);
    npc1->pos = {100, 100};
    npc1->speed = 10.0f;
    _actors.push_back(npc1);
    */

    auto npc2 = std::make_shared<NPC>("li");
    auto bev_2 = BrainTree::Builder()
        .composite<BrainTree::Sequence>()
        .leaf<Move>("Move2", npc2.get())
        .leaf<Resize>("Resize2", npc2.get())
        .end().build();

    npc2->setBevTree(bev_2);
    npc2->pos = {200, 100};
    npc2->speed = 10.0f;
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

    for(auto& actor : _actors)
    {
        actor->update();
    }
}

void SamplePluginBevTree::onDraw() 
{
    for(auto& actor : _actors)
    {
        actor->draw(application()->renderer());
    }
}

}