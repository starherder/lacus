#include "system_bubble.h"
#include "game/game_config.h"
#include "game/logic_config.h"
#include "utility/translator.h"
#include "engine/animation.h"


namespace game
{
namespace
{
    std::string makeBubbleEmotionAnimInstName(entt::entity actor, const std::string& emotion)
    {
        return "bubble-emotion-" + std::to_string(entt::to_integral(actor)) + "-" + emotion;
    }

    void removeBubbleEmotionAnim(GameContext& context, CompBubble& bubble)
    {
        if (bubble.emotion_anim_inst.empty())
        {
            return;
        }

        context.resourceMgr().animationManager().remove(bubble.emotion_anim_inst);
        bubble.emotion_anim_inst.clear();
        bubble.emotion_anim = nullptr;
    }
}

DeclareEcsSystem(BubbleSystem, EcsPriority::Middle);

BubbleSystem::BubbleSystem(GameContext& context) : EcsSystem(context)
{
    _context.dispatcher().sink<EvtShowBubble>().connect<&BubbleSystem::onShowBubble>(this);
}

BubbleSystem::~BubbleSystem()
{
}

void BubbleSystem::update(float delta)
{
    auto& registry = _context.registry();
    auto view = registry.view<CompBubble>();
    for (auto ent : view)
    {
        auto& bubble = view.get<CompBubble>(ent);

        int fadeout_ms = _context.gameConfig().bubble.fadeout_ms;
        int lifetime_ms = _context.gameConfig().bubble.lifetime_ms;

        bubble.lifetime -= _context.deltaTicks();
        if (bubble.lifetime < 0)
        {
            removeBubbleEmotionAnim(_context, bubble);
            registry.remove<CompBubble>(ent);
            continue;
        }

        if (bubble.lifetime < fadeout_ms)
        {
            float ratio = (float)bubble.lifetime / (float)fadeout_ms;
            bubble.alpha = (uint8_t)(ratio * 255.0f);
        }
    }
}

void BubbleSystem::onShowBubble(const EvtShowBubble& e)
{
    auto& registry = _context.registry();
    if (!registry.valid(e.actor))
    {
        return;
    }

    if (auto* oldBubble = registry.try_get<CompBubble>(e.actor))
    {
        removeBubbleEmotionAnim(_context, *oldBubble);
    }

    auto& bubble = registry.emplace_or_replace<CompBubble>(e.actor);

    // 文字（译文）
    if (!e.text.empty())
    {
        bubble.text = Trans(e.text);
    }
    else
    {
        bubble.text.clear();
    }

    // 字体
    auto& font_name = _context.gameConfig().bubble.font_name;
    auto& font_size = _context.gameConfig().bubble.font_size;
    bubble.font = _context.fontMgr().get(HashString(font_name.c_str()), font_size);

    // 表情
    if (!e.emotion.empty())
    {
        const EmotionConfig* emotion = _context.logicConfig().getEmotion(e.emotion);
        if (emotion)
        {
            if (emotion->type == EmotionResourceType::Animation)
            {
                bubble.emotion_anim_inst = makeBubbleEmotionAnimInstName(e.actor, e.emotion);
                bubble.emotion_anim = _context.resourceMgr().animationManager().create(emotion->resource, bubble.emotion_anim_inst);
                if (bubble.emotion_anim)
                {
                    bubble.emotion_anim->setLoop(true);
                    bubble.emotion_anim->play();
                }
            }
            else
            {
                bubble.emotion_tex = _context.textureMgr().getCfgTexTile(emotion->resource);
            }
        }
    }

    // 颜色
    bubble.bg_color = _context.gameConfig().bubble.bg_color;
    bubble.border_color = _context.gameConfig().bubble.border_color;
    bubble.text_color = _context.gameConfig().bubble.text_color;

    // 生命周期
    bubble.lifetime = _context.gameConfig().bubble.lifetime_ms;
    bubble.alpha = 255;
}

}
