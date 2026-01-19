#pragma once

#include "game/ui/form_logic_base.h"

namespace game 
{
    using namespace ui;

    class FormEntry : public FormLogicBase
    {
    public:
        signals::Signal<> on_start_game;
        signals::Signal<> on_resume_game;
        signals::Signal<> on_config_game;
        signals::Signal<> on_exit_game;

    public:
        FormEntry(const std::string& name) = delete;
        FormEntry(const std::string& name, GameContext& context);
        ~FormEntry();

    private:
        void onStart(Button* btn);

        void onResume(Button* btn);

        void onConfig(Button* btn);

        void onExit(Button* btn);

        void onUpdate(float delta) override;

        void onSizeChanged() override;

    private:
        ui::Label* _lblTitle = nullptr;
        ui::Button* _btnStart = nullptr;
        ui::Button* _btnResume = nullptr;
        ui::Button* _btnConfig = nullptr;
        ui::Button* _btnExit = nullptr;
    };
}