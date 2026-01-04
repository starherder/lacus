#include "sample_plugin_bevtree.h"

#include "bevtree/bevtree.h"

namespace samples {

using ActorPtr = std::shared_ptr<Actor>;

float VERY_NEAR = 5.0f;

class BrRole : public Actor
{
public:
    BrRole() = default;
    ~BrRole() = default;
    BrRole(int id_)
    {
        id = id_;
        color.fromHSV({(float)(rand()%360), 1.0f, 1.0f, 1.0f});
        //speed = 100+(float)(rand()%100);
        pos = {50+rand()%1000, 50+rand()%900};
        level = rand()%3;
        color.fromHSV({(float)(rand()%360), 1.0f, 1.0f, 1.0f});
    }

    void setBevTree(bevtree::BehaviorTree::Ptr bevTree){
        _bevTree = bevTree;
    }

    void update() override{
        _bevTree->update();
    }

    void draw(Renderer& renderer){
        Vec2 sz = size + Vec2{level*3, level*3};

        renderer.setDrawColor(color);
        renderer.drawFillRect({(pos - sz / 2.0f), sz });

        renderer.setDrawColor(Color::Gray);
        renderer.drawDebugText((pos - sz / 3.0f), std::to_string(level).c_str());
    }

    int id=0;
    std::string name;
    int level = 0;
    float speed = 100;

    Vec2 pos = { 200, 200 };
    Vec2 size = {30, 30};
    Vec2 vel = { 0, 0 };
    Color color = Color::Red;

private:
    bevtree::BehaviorTree::Ptr _bevTree = nullptr;
};

using BrRolePtr = std::shared_ptr<BrRole>;

class BrScene {
public:

    void addRole(BrRolePtr role)
    {
        roles[role->id] = role;
    }

    void removeRole(int id)
    {
        pending_role.push_back(id);
    }

    BrRole* getNearsetPrey(BrRole* me) {
        float dis = 99999999.0f;
        BrRole* prey = nullptr;
        for(auto& [id, role] : roles) {
            auto preydis = glm::distance(role->pos, me->pos);
            if( role.get() != me && role->level <= me->level && preydis < dis) {
                dis = preydis;
                prey = role.get();
            }
        }
        return prey;
    }

    BrRole* getEnenyNear(BrRole* me) {
        for (auto& [id, role] : roles) {
            auto enemydis = glm::distance(role->pos, me->pos);
            if( role.get() != me && role->level > me->level && enemydis < 50 ) {
                return role.get();
            }
        }
        return nullptr;
    }

    bool inScene(const Vec2& pos) {
        return pos.x > 0 && pos.x < size.x && pos.y > 0 && pos.y < size.y;
    }

    void update(float delta)
    {
        delta_time = delta;

        for(auto id : pending_role) {
            roles.erase(id);
        }
        pending_role.clear();

        for(auto& [id,role] : roles) {
            role->update();
        }
    }

    void draw(Renderer& renderer)
    {
        for (auto& [id, role] : roles) {
            role->draw(renderer);
        }
    }

    std::map<int, BrRolePtr>  roles;
    std::vector<int> pending_role;

    Vec2 size = {1280, 1024};
    float delta_time = 0.0f;
};


BrScene g_scene;


class BrEnemySensor : public bevtree::BevNode
{
public:
    Status update() override
    {
        BrRole* role = getBlackboard()->getValue<BrRole*>("role", nullptr);
        BrScene* scene = getBlackboard()->getValue<BrScene*>("scene", nullptr);
        if(!role || !scene) {
            spdlog::error("pointer is nullptr");
            return Node::Status::Failure;
        }

        auto enemy = scene->getEnenyNear(role);
        if(enemy) {
            auto dest = role->pos + Vec2{-enemy->vel.y, enemy->vel.x} * 100.0f;
            if(!scene->inScene(dest)) {
                dest = role->pos + Vec2{ enemy->vel.y, -enemy->vel.x } *100.0f;
            }

            getBlackboard()->set("destination", dest);
        }
        return enemy ? Node::Status::Success : Node::Status::Failure;
    }
};
RegisterBehaviorNode("br_enemy_sensor", BrEnemySensor);


class BrPreySensor : public bevtree::BevNode
{
public:
    Status update() override
    {
        BrRole* role = getBlackboard()->getValue<BrRole*>("role", nullptr);
        BrScene* scene = getBlackboard()->getValue<BrScene*>("scene", nullptr);
        if (!role || !scene) {
            spdlog::error("pointer is nullptr");
            return Node::Status::Failure;
        }

        auto prey = scene->getNearsetPrey(role);
        if (prey) {
            getBlackboard()->set("destination", prey->pos);
        }
        return prey ? Node::Status::Success : Node::Status::Failure;
    }
};
RegisterBehaviorNode("br_prey_sensor", BrPreySensor);


class BrMoveTo : public bevtree::BevNode
{
public:
    void initialize() override {
        _role = getBlackboard()->getValue<BrRole*>("role", nullptr);
        _destination = getBlackboard()->getValue<Vec2>("destination", {0, 0});
        _role->vel = glm::normalize(_destination - _role->pos);
    }

    void terminate(Status s) override {
    }

    Status update() override {
        _role->pos += _role->vel * _role->speed * g_scene.delta_time;

        if (glm::length(_role->pos - _destination) < VERY_NEAR) {
            _role->pos = _destination;
            return Node::Status::Success;
        }

        if (_role->pos.x < 50 || _role->pos.x > g_scene.size.x - 50
            || _role->pos.y < 50 || _role->pos.y > g_scene.size.y - 50)
        {
            return Node::Status::Failure;
        }

        return Node::Status::Running;
    }

private:
    BrRole* _role = nullptr;
    Vec2 _destination;
};
RegisterBehaviorNode("br_move", BrMoveTo);


class BrKillPrey : public bevtree::BevNode
{
public:
    Status update() override
    {
        BrRole* role = getBlackboard()->getValue<BrRole*>("role", nullptr);
        BrScene* scene = getBlackboard()->getValue<BrScene*>("scene", nullptr);
        if (!role || !scene) {
            spdlog::error("pointer is nullptr");
            return Node::Status::Failure;
        }

        auto prey = scene->getNearsetPrey(role);
        if (prey) {
            if(glm::distance(role->pos, prey->pos) < VERY_NEAR ){
                
                bool role_win = false;
                if(role->level > prey->level) {
                    role_win = true;
                } else if(role->level < prey->level) {
                    role_win = false;
                } else {
                    role_win = (rand()%2 == 0);
                }

                if(role_win) {
                    role->level ++;
                    g_scene.removeRole(prey->id);
                }
                else {
                    prey->level ++;
                    g_scene.removeRole(role->id);
                }
            }
        }
        return Node::Status::Success;
    }
};
RegisterBehaviorNode("br_kill", BrKillPrey);



void SamplePluginBevTree::initBattleRoyale()
{
    auto filepath = application()->resPath() / "roles/bevtree/bev_battle_royale.xml";
    bevtree::BevTreeManager::inst().load(filepath);

    for(int i=0; i<50; i++)
    {
        auto btree = bevtree::BevTreeManager::inst().createBevTree("common_role");
        if(btree) {
            auto actor = std::make_shared<BrRole>(i);
            btree->getBlackboard()->set("role", actor.get());
            btree->getBlackboard()->set("scene", &g_scene);
            actor->setBevTree(btree);

            g_scene.addRole(actor);
        }
    }
}

void SamplePluginBevTree::updateBattleRoyale()
{
    g_scene.update(_frameSeconds);
}


void SamplePluginBevTree::drawBattleRoyale(Renderer& renderer)
{
    g_scene.draw(renderer);
}









}