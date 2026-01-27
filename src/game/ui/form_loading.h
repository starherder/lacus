#pragma once

#include "game/ui/form_logic_base.h"

namespace game 
{

    class FormLoading : public FormLogicBase
    {
    public:
        FormLoading(const std::string& name) = delete;
        FormLoading(const std::string& name, GameContext& context);
        ~FormLoading();

        void setProgress(float progress);

    private:
        void onUpdate(float delta) override;

        void onSizeChanged() override;
    };
}