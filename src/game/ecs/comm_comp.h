#pragma once

#include "entt/entt.hpp"
#include "engine/wrapper.h"
#include "engine/texture.h"

namespace bevtree {
    class BehaviorTree;
}

namespace game 
{
    using namespace engine;

    struct CompNameId
    {
        entt::entity id;
        std::string name;
    };

    struct CompTransform
    {
        Vec2 position;
        Vec2 size;
        Vec2 rotation;
        Vec2 scale;
    };

    enum class MotionState {
        Moving,
        Resting,
        Paused,
    };

    struct CompMotion
    {
        MotionState state = MotionState::Resting;

        Vec2 targetPos;
        Vec2 velocity;
        float speed = 50.0f;

        std::list<Vec2i> path;
    };

    struct CompDisplay
    {
        Color ground_color = Color::Yellow;
        Color border_color = Color::Dark;
        Texture* texture = nullptr;
        Rect texture_rect;
    };

    struct CompNpcPatrol {
        Vec2 origin_pos;
        float patrol_radius;
    };

    struct CompBevtree
    {
        std::shared_ptr<bevtree::BehaviorTree> bevtree = nullptr;
    };

}
