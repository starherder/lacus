#include "form_config.h"


namespace game
{

    FormConfig::FormConfig(const std::string& name, GameContext& context) : FormLogicBase(name, context)
    {
        load(_context.resPath() / "ui/form_config.xml");

        auto btn_confirm = getWidget<ui::Button>("btn_confirm");
        if (btn_confirm)
        {
            btn_confirm->on_click.connect([this](ui::Button* btn) { close(); });
        }

        auto btn_cancel = getWidget<ui::Button>("btn_cancel");
        if (btn_cancel)
        {
            btn_cancel->on_click.connect([this](ui::Button* btn) { close(); });
        }

        auto slider_sound = getWidget<ui::SliderBar>("slider_sound");
        if (slider_sound)
        {
            slider_sound->on_value_changed.connect([this](ui::SliderBar* bar) {
                _context.audioPlayer().setSoundVolume(bar->value() / 100.0f);
            });
        }

        auto slider_music = getWidget<ui::SliderBar>("slider_music");
        if (slider_music)
        {
            slider_music->on_value_changed.connect([this](ui::SliderBar* bar) {
                _context.audioPlayer().setMusicVolume(bar->value() / 100.0f);
            });
        }
    }

    FormConfig::~FormConfig()
    {
    }

    void FormConfig::onUpdate(float delta)
    {
    }

    void FormConfig::onSizeChanged()
    {
    }
}