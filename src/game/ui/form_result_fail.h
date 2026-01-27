#pragma once

#include "game/ui/form_logic_base.h"

namespace game 
{

    class FormResultFail : public FormLogicBase
    {
    public:
        FormResultFail(const std::string& name) = delete;
        FormResultFail(const std::string& name, GameContext& context);
        ~FormResultFail();

    private:
        void onUpdate(float delta) override;

        void onSizeChanged() override;
    };
}