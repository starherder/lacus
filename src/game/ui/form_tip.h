#pragma once

#include "game/ui/form_logic_base.h"

namespace game
{
    enum class TipMode
    {
        Ok,
        ConfirmCancel,
    };

    class FormTip : public FormLogicBase
    {
    public:
        signals::Signal<> on_confirm;
        signals::Signal<> on_cancel;
        signals::Signal<> on_ok;
        
    public:
        FormTip(const std::string& name, GameContext& context);

        void setTipMode(TipMode mode);
        void setText(const std::string& text);

    private:
        TipMode _mode = TipMode::Ok;
    };
}
