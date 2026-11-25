#pragma once

#include "game/ui/form_logic_base.h"

namespace game 
{
    using namespace ui;

    class FormStart : public FormLogicBase
    {
    public:
        FormStart(const std::string& name) = delete;
        FormStart(const std::string& name, GameContext& context);
        ~FormStart();

    private:
        void onStart(Button* btn);

        void onTileBattle(Button* btn);
        
        void onAutoChess(Button* btn);

        void onConfig(Button* btn);

        void onExit(Button* btn);

        void onUpdate(float delta) override;

        void onSizeChanged() override;

    };
}