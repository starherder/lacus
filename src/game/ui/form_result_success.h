#pragma once

#include "game/ui/form_logic_base.h"

namespace game 
{

    class FormResultSuccess : public FormLogicBase
    {
    public:
        FormResultSuccess(const std::string& name) = delete;
        FormResultSuccess(const std::string& name, GameContext& context);
        ~FormResultSuccess();

    private:
        void onUpdate(float delta) override;

        void onSizeChanged() override;
    };
}