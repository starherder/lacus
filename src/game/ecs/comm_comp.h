#pragma once

#include "entt/entt.hpp"
#include "engine/wrapper.h"
#include "engine/texture.h"

#include "utility/dynamic_struct.h"

#include "tweeny/tweeny.h"
#include "particle/particle_manager.h"
#include "bevtree/bevtree.h"

namespace engine
{
    class Animation;
}

namespace game 
{
    using namespace engine;

    enum class ObjectType {
        Item,
        Npc,
        Spawner,

        Other,
        All,
    };

    enum class CampSide {
        None,
        Officer,
        Rebel,
        Gangster,
        Civilian,
    };

    enum class ArmRank {
        Leader, // 头领
        Soldier, // 士兵
    };

    enum class ArmProfession
    {
        Commander, // 指挥
        Adviser, // 军师
        Medic, // 医生
        Rider, // 骑兵
        Foot, // 步兵
        Archer, // 弓箭手
        Wizard, // 法师
        Artisan, // 技工
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

    enum class CoordMode {
        WorldSpace,
        ScreenSpace,
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

    enum class ShapeType
    {
        Circle,
        Rect,
        Invalid
    };

    struct CompNameId
    {
        entt::entity id;
        std::string name;
        std::string cfg_id;
    };

    struct CompDead 
    {
        entt::entity killer;
        int ticks = 0;
    };

    struct CompRoleEvent
    {
        std::string dead;
        std::string born;
    };

    struct CompDestroy 
    {
    };

    struct CompComm 
    {
        ObjectType type;
        std::string desc;
        CampSide side;
        ArmRank rank;
        //ArmProfession pro;
    };

    struct CompTransform
    {
        Vec2 position = { 0.0f, 0.0f };
        Vec2 size = { 0.0f, 0.0f };
        Vec2 base_size = { 0.0f, 0.0f };
        Vec2 visual_offset = { 0.0f, 0.0f };
        Vec2 rotation = { 0.0f, 0.0f };
        Vec2 scale = { 1.0f, 1.0f };

        CoordMode coord_mode = CoordMode::WorldSpace;
    };

    struct CompMoveCfg
    {
        float speed = 150.0f;
        float swim_speed = 150.0f;
    };

    struct CompAutoMotion
    {
        MotionState state = MotionState::Resting;

        Vec2i targetGrid;
        tweeny::tween<float, float> tween;

        std::vector<Vec2i> path;
        std::vector<Vec2i>::reverse_iterator path_iterator;
    };

    struct CompStepMotion
    {
        MotionState state = MotionState::Resting;
        tweeny::tween<float, float> tween;
    };

    struct CompGameTurn
    {
        bool running = false;
    };

    struct CompSelection
    {
        int ticks = 0;
        int dir = 1;
    };

    struct CompAutoPick
    {
        int ticks = 0;
    };

    struct CompBindParticle 
    {
        particle::ParticlePtr particle = nullptr;
    };

    struct CompDisplay
    {
        bool visible = true;

        Color ground_color = Color::Yellow;
        Color border_color = Color::Dark;
        Color font_color = Color::Dark;

        Font* font = nullptr;
        TexTile* texture = nullptr;
        TexTile* ground_texture = nullptr;
    };

    struct CompMarkDisplay
    {
        ShapeType shape_type = ShapeType::Rect;

        Color ground_color = Color::Yellow;
        Color border_color = Color::Dark;

        Texture* texture = nullptr;
        Rect tex_rect;
    };

    struct CompBubble
    {
        std::string text;                    // 气泡文字（已译文）
        Font* font = nullptr;                // 文字字体
        TexTile* emotion_tex = nullptr;      // 静态表情纹理
        Animation* emotion_anim = nullptr;   // 动态表情动画
        std::string emotion_anim_inst;       // 动态表情动画实例名
        Color bg_color = Color::White;
        Color border_color = Color::Dark;
        Color text_color = Color::Dark;
        uint8_t alpha = 255;                 // 当前不透明度
        int lifetime = 3000;                 // 剩余毫秒
    };

    struct CompStoryPlayer
    {
        std::string story_name;
        std::map<std::string, entt::entity> actors;
        size_t dialogue_index = 0;
        int dialogue_ticks = 0;
        bool camera_follow = false;
        bool game_pause = false;
        entt::entity camera_target = entt::null;
        Vec2 camera_target_pos = { 0.0f, 0.0f };
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

    struct CompPickable 
    {
        int amount = 0;
        std::string effect;
        std::string funcs;
        int use_ticks = 1000;
        bool pick_use = false;

        bool picked = false;
        tweeny::tween<float, float> tween;
    };

    struct CompDragging 
    {
        Vec2 origin_pos;
        Vec2 tip_pos;
        Vec2 tip_size;
        Color ground_color = Color::LightRed;
        Color border_color = Color::Red;
        float border_size = 1.0f;
    };

    struct CompAudio 
    {
        std::string audio_name;
    };


    struct CompSkyEffect
    {
        SkyEffect effect;

        Color color;

        tweeny::tween<int> tween;
    };

    struct CompSpawner
    {
        std::string npc;

        float radius = 100.0f;
        int interval = 1000;

        int min_count = 1; // spawn immediately when npc_count < min_count
        int max_count = 3; // stop spawn where npc_count >= max_count

        int64_t cur_tick = 0;
        std::set<entt::entity> npc_set;
    };
    
    struct CompSpawnMe
    {
        entt::entity who_spawn_me;
    };


    inline ObjectType getNpcType(const std::string& npctype) 
    {
        if (npctype == "item") return ObjectType::Item;
        if (npctype == "npc") return ObjectType::Npc;
        if (npctype == "spawner") return ObjectType::Spawner;
        if (npctype == "other") return ObjectType::Other;

        return ObjectType::Other;
    };

    inline CampSide getCampSide(const std::string& side)
    {
        if (side == "official") return CampSide::Officer;
        if (side == "civilian") return CampSide::Civilian;
        if (side == "rebel") return CampSide::Rebel;
        if (side == "gangster") return CampSide::Gangster;

        return CampSide::None;
    };


}
