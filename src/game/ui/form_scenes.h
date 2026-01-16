#pragma once

#include "game/ui/form_logic_base.h"

namespace game 
{
    using namespace ui;

    class FormScenes : public FormLogicBase
    {
    public:
        FormScenes(const std::string& name) = delete;
        FormScenes(const std::string& name, GameContext& context);
        ~FormScenes();

    private:
        void onUpdate(float delta) override;

        void onSelectScene(ui::Button* btn);
    };
}