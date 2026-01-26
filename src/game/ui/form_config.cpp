#include "form_config.h"


namespace game
{

    FormConfig::FormConfig(const std::string& name, GameContext& context) : FormLogicBase(name, context)
    {
        _vlay_bg = root()->createChild<ui::VLayout>("vlay_main");
        _vlay_bg->setBgColor(Color::DarkCyan);

        {
            auto hlay_title = _vlay_bg->createChild<ui::HLayout>("hlay_title");
            hlay_title->setSize({ 0, 50 });

            hlay_title->createChild<Widget>("space_0");

            auto btn_close = hlay_title->createChild<ui::Button>("btn_close");
            btn_close->setText("X");
            btn_close->setSize({ 100, 0 });
            btn_close->on_click.connect(this, &FormConfig::onBtnClose);
        }

        {
            auto vlay_sound = _vlay_bg->createChild<ui::VLayout>("vlay_sound");
            vlay_sound->createChild<ui::Widget>("top_blank");
            {
                auto hlay_sound = vlay_sound->createChild<ui::HLayout>("hlay_sound");
                {
                    hlay_sound->setSize({0, 50});
                    {
                        hlay_sound->createChild<ui::Widget>("left_blank");
                        hlay_sound->createChild<ui::Label>("lbl_sound")->setText("sound");
                        hlay_sound->createChild<ui::SliderBar>("sld_sound")->on_value_changed.connect([this](ui::SliderBar* bar) {
                            _context.audioPlayer().setSoundVolume(bar->value() / 100.0f);
                        });
                        hlay_sound->createChild<ui::Widget>("right_blank");
                    }
                    vlay_sound->createChild<ui::Widget>("mid_blank")->setSize({0, 30});
                }
            
            
                auto hlay_music = vlay_sound->createChild<ui::HLayout>("hlay_music");
                {
                    hlay_music->setSize({ 0, 50 });
                    {
                        hlay_music->createChild<ui::Widget>("left_blank");
                        hlay_music->createChild<ui::Label>("lbl_music")->setText("music");
                        hlay_music->createChild<ui::SliderBar>("sld_music")->on_value_changed.connect([this](ui::SliderBar* bar) {
                            _context.audioPlayer().setMusicVolume(bar->value() / 100.0f);
                        });
                        hlay_music->createChild<ui::Widget>("right_blank");
                    }
                    vlay_sound->createChild<ui::Widget>("bottom_blank");
                }
            }
        }

        {
            auto hlay_func = _vlay_bg->createChild<ui::HLayout>("hlay_funcs");
            {
                hlay_func->setSize({ 0, 50 });

                hlay_func->createChild<ui::Widget>("left_blank");

                auto btn_confirm = hlay_func->createChild<ui::Button>("btn_confirm");
                btn_confirm->setSize({ 100, 0 });
                btn_confirm->setText("confirm");
                btn_confirm->on_click.connect([this](ui::Button* btn) { close(); });

                hlay_func->createChild<ui::Widget>("black");

                auto btn_cancel = hlay_func->createChild<ui::Button>("btn_cancel");
                btn_cancel->setSize({ 100, 0 });
                btn_cancel->setText("cancel");
                btn_cancel->on_click.connect([this](ui::Button* btn) { close(); });

                hlay_func->createChild<ui::Widget>("right_blank");
            }
        }

        _vlay_bg->createChild<ui::Widget>("wdget_bottom")->setSize({0, 100});

        setMaximize(true);
        setDragMovable(false);
    }

    FormConfig::~FormConfig()
    {
    }

    void FormConfig::onBtnClose(ui::Button* btn)
    {
        close();
    }

    void FormConfig::onUpdate(float delta)
    {
    }

    void FormConfig::onSizeChanged()
    {
    }
}