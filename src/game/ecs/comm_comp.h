#pragma once

#include "entt/entt.hpp"
#include "engine/wrapper.h"
#include "engine/texture.h"

#include "utility/dynamic_struct.h"

#include "tweeny/tweeny.h"

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
        std::string cfg_id;
    };


    struct CompComm {
        std::string type;
        std::string desc;

        enum class CampSide {
            Officer,
            Foreign,
            Rebel,
            Gangster,
        };
        CampSide comp = CampSide::Gangster;
    };

    struct CompTransform
    {
        Vec2 position;
        Vec2 size;
        Vec2 rotation;
        Vec2 scale;
    };

    enum class MotionState 
    {
        Moving,
        Resting,
        Paused,
    };

    struct CompMotion
    {
        float speed = 150.0f;

        MotionState state = MotionState::Resting;

        Vec2i targetGrid;

        std::string tween_mode = "linear";
        tweeny::tween<float,float> tween;

        std::vector<Vec2i> path;
        std::vector<Vec2i>::reverse_iterator path_iterator;
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

    // 预设：智力、灵巧、灵性、力量等预设值
    struct CompPresets
    {
        utility::DynamicStruct<std::string> properties;
    };

    // HP、攻击力、防御力、移动速度、攻击速度等经过公式计算后得到的属性
    struct CompProps
    {
        utility::DynamicStruct<std::string> properties;
    };

    struct CompSkills {
        std::vector<std::string> skills;
    };
    
    struct CompBuffs {
        std::vector<std::string> buffs;
    };
    
    struct CompItems {
        std::vector<std::string> items;
    };


    struct CompNpcPatrol 
    {
        Vec2 origin_pos;
        float patrol_radius;
    };

    struct CompBevtree
    {
        std::shared_ptr<bevtree::BehaviorTree> bevtree = nullptr;
    };

}
