#pragma once

#include "entt/entt.hpp"
#include "engine/wrapper.h"
#include "engine/texture.h"

#include "tweeny/tweeny.h"
#include "particle/particle_manager.h"
#include "game/ecs/comm_comp.h"

namespace game
{
    struct CompEffect
    {
        tweeny::tween<int> tween;
    };

    struct CompEffectAudio
    {
        std::string audio_file;
    };

    struct CompEffectAnimation
    {
        //std::vector<Texture*> textures;
        //tweeny::tween<float, float> tween;
    };
    
    struct CompEffectMotion
    {
        tweeny::tween<float, float> tween;
    };

    struct CompEffectScale
    {
        tweeny::tween<float, float> tween;
    };

    struct CompEffectColor
    {
        tweeny::tween<int, int, int, int> tween;
    };


}