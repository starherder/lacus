#pragma once

#include "game/ui/form_logic_base.h"
#include "ui/game_widgets.h"

namespace game 
{

    class FormCards : public FormLogicBase
    {
    public:
        FormCards(const std::string& name) = delete;
        FormCards(const std::string& name, GameContext& context);
        ~FormCards();

    private:
        void onUpdate(float delta) override;

        void onSizeChanged() override;

        void loadCars();

    private:
        ui::CardGroup* _cardGroup = nullptr;
    };
}