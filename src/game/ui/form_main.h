#pragma once

#include "game/ui/form_logic_base.h"

namespace game {
    using namespace ui;

    class FormMain : public FormLogicBase
    {
    public:
        FormMain(const std::string& name, GameContext& context);
        ~FormMain();

    private:
        void onUpdate(float delta) override;

    private:
        ui::Button* _btnScenes = nullptr;
    };
}