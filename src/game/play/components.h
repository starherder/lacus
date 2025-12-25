#pragma once

#include "entt/entt.hpp"
#include "engine/wrapper.h"
#include "engine/texture.h"

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

    struct CompMotion
    {
        bool running = false;
        float speed = 50.0f;
        Vec2 velocity;

        Vec2 targetPos;
        std::list<Vec2i> path;
    };

    struct CompDisplay
    {
        Color ground_color = Color::Yellow;
        Color border_color = Color::Dark;
        Texture* texture = nullptr;
        Rect texture_rect;
    };

    enum TweenEaseMode {Once, Loop, Yoyo};
    struct ComMotionTween
    {
        std::string ease_type;
        Vec2 source;
        Vec2 target;
        int duration;
    };

    enum class ActorState {
        Idle,
        Move,
        Attack,
        Die,
    };

    struct CompState
    {
        ActorState state = ActorState::Idle;
    };

}
