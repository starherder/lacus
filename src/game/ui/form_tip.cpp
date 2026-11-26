#include "form_tip.h"

namespace game
{
    FormTip::FormTip(const std::string& name, GameContext& context)
        : FormLogicBase(name, context)
    {
        load(_context.resPath() / "ui/form_tip.xml");

        if (auto button = getWidget<ui::Button>("btn_confirm"))
        {
            button->on_click.connect([this](ui::Button*) {
                if (_mode == TipMode::ConfirmCancel)
                {
                    on_confirm.emit();
                    close();
                }
            });
        }

        if (auto button = getWidget<ui::Button>("btn_cancel"))
        {
            button->on_click.connect([this](ui::Button*) {
                if (_mode == TipMode::ConfirmCancel)
                {
                    on_cancel.emit();
                    close();
                }
            });
        }

        if (auto button = getWidget<ui::Button>("btn_ok"))
        {
            button->on_click.connect([this](ui::Button*) {
                if (_mode == TipMode::Ok)
                {
                    on_ok.emit();
                    close();
                }
            });
        }

        setTipMode(TipMode::Ok);
    }

    void FormTip::setTipMode(TipMode mode)
    {
        _mode = mode;
        const bool confirmCancel = mode == TipMode::ConfirmCancel;

        if (auto group = getWidget<ui::Group>("hlay_confirm_cancel"))
        {
            group->setVisible(confirmCancel);
        }

        if (auto group = getWidget<ui::Group>("hlay_ok"))
        {
            group->setVisible(!confirmCancel);
        }
    }

    void FormTip::setText(const std::string& text)
    {
        if (auto label = getWidget<ui::Label>("lbl_tip"))
        {
            label->setText(text);
        }
    }
}
