#pragma once

#include "game/ui/form_logic_base.h"

namespace game 
{
    using namespace ui;

    class FormEntry : public FormLogicBase
    {
    public:
        FormEntry(const std::string& name) = delete;
        FormEntry(const std::string& name, GameContext& context);
        ~FormEntry();

        void selectScene(const std::string& scene);
        
    private:
        void onStart(Button* btn);
        
        void onShowCardDeck(Button* btn);

        void onUpdate(float delta) override;

        void onSizeChanged() override;

        void onWindowResized(const Vec2& size);

        void onDropCard(ui::GuiManager::DraggingPtr ptr);
        
        void showCardGroup();

        void resize();

    private:
        std::string _sceneFile;
    };
}