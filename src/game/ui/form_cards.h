#pragma once

#include "game/ecs/comm_comp.h"
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

        signals::Signal<> onHandCardsConfirmed;

    private:
        void onUpdate(float delta) override;

        void onSizeChanged() override;

        void onDropCard(ui::GuiManager::DraggingPtr ptr);

        bool onDropRankMarker(ui::GuiManager::DraggingPtr ptr);

        void onConfirm(ui::Button* btn);
        
        void loadCards();

        void attachRankMarker(ui::CardWidget* card, ui::WidgetPtr marker);

        ui::WidgetPtr takeRankMarker(RoleRank rank);

        void returnRankMarker(ui::WidgetPtr marker);
        void returnCardRankMarkers(ui::CardWidget* card);

        ui::CardWidget* findGridCardAt(const Vec2& pos);
        
        static const char* rankMarkerName(RoleRank rank);
        static RoleRank markerRank(ui::Label* marker);

    private:
        ui::CardGroup* _cardGroup = nullptr;
        ui::GridGroup* _gridGroup = nullptr;
        ui::Group* _toolsGroup = nullptr;
    };
}
