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

        void selectScene(int index, const std::string& scene);
        
    private:
        void onStart(Button* btn);

        void onUpdate(float delta) override;

        void onSizeChanged() override;

        void onWindowResized(const Vec2& size);

    private:
        int _sceneIndex;
        std::string _sceneFile;
    };
}