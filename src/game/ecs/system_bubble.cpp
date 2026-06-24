#include "system_bubble.h"
#include "game/game_config.h"
#include "game/logic_config.h"
#include "utility/translator.h"


namespace game
{

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
        const std::string* texRef = _context.logicConfig().getEmotion(e.emotion);
        if (texRef)
        {
            bubble.emotion_tex = _context.textureMgr().getCfgTexTile(*texRef);
        }
        else
        {
            bubble.emotion_tex = nullptr;
        }
    }
    else
    {
        bubble.emotion_tex = nullptr;
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
