#pragma once

#include "entt/entt.hpp"
#include "engine/wrapper.h"
#include "engine/texture.h"

namespace game 
{
    using namespace engine;

    struct CompNameId
    {
        IdType id;
        std::string name;
    };

    struct CompTransform
    {
        Vec2 position = {1, 1};
        Vec2 size = {64, 64};
        Vec2 rotation = {0, 0};
        Vec2 scale = {1, 1};
    };

    struct CompDisplay
    {
        Color ground_color = Color::Light;
        Color border_color = Color::Black;

        Texture* texture = nullptr;
        Rect texture_rect = {0,0,1,1};
    };

    enum class ActorState {
        Idle,
        Move,
        Attack,
        Die,
    };

    struct CompState {
        ActorState state = ActorState::Idle;
    };

    struct ComMotion
    {
        Vec2 velocity = {0, 0};

        std::vector<Vec2> path;
    };

    enum TweenEaseMode {Once, Loop, Yoyo};
    struct ComMotionTween
    {
        std::string ease_type;
        Vec2 source;
        Vec2 target;
        int duration;
    };

}
