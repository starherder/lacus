#pragma once

#include "entt/entt.hpp"
#include "engine/wrapper.h"
#include "engine/texture.h"

#include "utility/dynamic_struct.h"

#include "tweeny/tweeny.h"
#include "particle/particle_manager.h"
#include "bevtree/bevtree.h"



namespace game 
{
    using namespace engine;

    struct CompNameId
    {
        entt::entity id;
        std::string name;
        std::string cfg_id;
    };

    enum class ObjectType {
        Item,
        Npc,

        Other,
    };

    enum class CampSide {
        Officer,
        Foreign,
        Rebel,
        Gangster,
    };

    struct CompDead {
        int ticks = 0;
    };

    struct CompDestroy {
    };

    struct CompComm {
        
        ObjectType type;

        std::string desc;

        CampSide comp = CampSide::Gangster;
    };

    enum class CoordMode {
        WorldSpace,
        ScreenSpace,
    };

    struct CompTransform
    {
        Vec2 position;
        Vec2 size;
        Vec2 rotation;
        Vec2 scale;

        CoordMode coord_mode = CoordMode::WorldSpace;
    };

    enum class MotionState 
    {
        Moving,
        Resting,
        Paused,
    };

    enum class MotionMode
    {
        Run,
        Swim,
        Ride,
    };

    struct CompMotion
    {
        float speed = 150.0f;

        MotionState state = MotionState::Resting;
        
        MotionMode mode = MotionMode::Run;

        Vec2i targetGrid;

        tweeny::tween<float, float> tween;

        std::vector<Vec2i> path;
        std::vector<Vec2i>::reverse_iterator path_iterator;
    };

    struct CompSelection
    {
        Color border_color = Color::Red;
        Color ground_color = {255,255,255,128};
        float border_size = 5.0f;
    };

    struct CompBindParticle 
    {
        particle::ParticlePtr particle = nullptr;
    };

    struct CompDisplay
    {
        Color ground_color = Color::Yellow;
        Color border_color = Color::Dark;
        Color font_color = Color::Dark;

        Font* font = nullptr;
        Texture* texture = nullptr;
        Rect tex_rect;
    };

    struct CompRolePick 
    {
        float range = 100.0f;
        std::vector<std::string> pick_types;
    };

    struct CompNpcPatrol 
    {
        Vec2 origin_pos;
        float patrol_radius;
    };

    struct CompBehavior
    {
        float vision_dis = 300.0f;
        std::shared_ptr<bevtree::BehaviorTree> bevtree = nullptr;
    };

    struct CompPickable {
        int amount = 0;

        bool picked = false;
        std::string effect;

        tweeny::tween<float, float> tween;
    };

    struct CompAudio 
    {
        std::string audio_name;
    };

    enum class SkyEffect
    {
        None,
        Dark,
        Fog,
        Cold,
        Hot,
        Rain,
    };

    struct CompSkyEffect
    {
        SkyEffect effect;

        Color color;

        tweeny::tween<int> tween;
    };


    inline  ObjectType getNpcType(const std::string& npctype) 
    {
        if (npctype == "item") return ObjectType::Item;
        if (npctype == "npc") return ObjectType::Npc;
        if (npctype == "other") return ObjectType::Other;

        spdlog::error("getNpcType: ({}) NOT support", npctype);
        return ObjectType::Other;
    };

    inline CampSide getCompSide(const std::string& side)
    {
        if (side == "official") return CampSide::Officer;
        if (side == "foreign") return CampSide::Foreign;
        if (side == "rebel") return CampSide::Rebel;
        if (side == "gangster") return CampSide::Gangster;
        spdlog::error("camp ({}) NOT support", side);
        return CampSide::Gangster;
    };


}
