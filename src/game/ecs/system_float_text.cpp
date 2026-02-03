#include "system_float_text.h"
#include "comp_fight.h"
#include "game/game_config.h"

namespace game 
{
    DeclareEcsSystem(FloatTextSystem, EcsPriority::Middle);

	FloatTextSystem::FloatTextSystem(GameContext& context) : EcsSystem(context)
	{
		_context.dispatcher().sink<EvtShowFloatText>().connect<&FloatTextSystem::onFloatTextStart>(this);
	}

	FloatTextSystem::~FloatTextSystem()
	{
	}

	void FloatTextSystem::update(float delta)
	{
		auto ent_view = _context.registry().view<CompFightText>();
		for (auto& ent : ent_view)
		{
			auto& compText = ent_view.get<CompFightText>(ent);
            compText.tween.step(_context.deltaTicks());
		}
	}


    std::string FloatTextSystem::getFloatText(const EvtShowFloatText& e)
    {
        if (e.type == FloatTextType::HP)
        {
            return std::format("HP{}{}", e.val > 0 ? "+" : "", e.val);
        }

        if (e.type == FloatTextType::LV)
        {
            return std::format("LV{}{}", e.val > 0 ? "+" : "", e.val);
        }

        return "OOO";
    }

    Color FloatTextSystem::getFloatColor(const EvtShowFloatText& e)
    {
        auto& colorinc = _context.gameConfig().float_text.color_inc;
        auto& colordec = _context.gameConfig().float_text.color_dec;

        return (e.val > 0) ? colorinc : colordec;
    }

    Font* FloatTextSystem::getFloatFont(const EvtShowFloatText& e)
    {
        auto& fontname = _context.gameConfig().float_text.font_name;
        auto& fontsize = _context.gameConfig().float_text.font_size;

        return _context.fontMgr().get(HashString(fontname.c_str()), fontsize);;
    }

	void FloatTextSystem::onFloatTextStart(const EvtShowFloatText& e)
	{
        auto& targetTrans = _context.registry().get<CompTransform>(e.actor);
        float curY = targetTrans.position.y;
        float dstY = targetTrans.position.y - 100;

        auto word = _context.registry().create();
        _context.registry().emplace<CompTransform>(word, targetTrans);
        _context.registry().emplace<CompFightText>(word, CompFightText{});

        auto& ft = _context.registry().get<CompFightText>(word);

        auto& tweenMode = _context.gameConfig().float_text.tween_mode;
        auto& floatticks = _context.gameConfig().float_text.float_ticks;

        ft.font = getFloatFont(e);
        ft.color = getFloatColor(e);

        ft.text = getFloatText(e);
        ft.tween = tweeny::from(curY, 255.0f)
            .to(dstY, 0.0f)
            .via(tweenMode)
            .during(floatticks)
            .onStep([this, word](auto& t, float y, float a) {
                if (t.isFinished()) 
                {
                    _context.scene().destroyObject(word);
                    return true;
                }

                auto& compTrans = _context.registry().get<CompTransform>(word);
                compTrans.position.y = y;

                auto& compFightText = _context.registry().get<CompFightText>(word);
                compFightText.color.a = (uint8_t)a;

                return false;
            });


	}
}

