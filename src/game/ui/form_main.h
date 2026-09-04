#pragma once

#include "game/ui/form_logic_base.h"

#include "ui/game_widgets.h"

namespace game {
    using namespace ui;

    class FormMain : public FormLogicBase
    {
    public:
        FormMain(const std::string& name, GameContext& context);
        ~FormMain();

    private:
        void onUpdate(float delta) override;

        void onDropCard(ui::GuiManager::DraggingPtr ptr);

        void onSizeChanged() override;

        void onWindowResized(const Vec2& size);

        void onLeaveScene(Button* btn);
        
        void onShowScenes(Button* btn);

        void onShowConfig(Button* btn);


    private:
        ui::Button* _btnScenes = nullptr;
        ui::Button* _btnConfig = nullptr;

        ui::CardGroup* _cardGroup = nullptr;
    };
}