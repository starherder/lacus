#pragma once

#include "game/ui/form_logic_base.h"

namespace game 
{

    class FormConfig : public FormLogicBase
    {
    public:
        FormConfig(const std::string& name) = delete;
        FormConfig(const std::string& name, GameContext& context);
        ~FormConfig();

    private:
        void onUpdate(float delta) override;

        void onSizeChanged() override;

        void onBtnClose(ui::Button* btn);

    private:
        ui::VLayout* _vlay_bg = nullptr ;
    };
}