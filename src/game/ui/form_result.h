#pragma once

#include "game/ui/form_logic_base.h"

namespace game 
{
    enum class GameResult {
        Success,
        Failed,
    };

    class FormResult : public FormLogicBase
    {
    public:
        FormResult(const std::string& name) = delete;
        FormResult(const std::string& name, GameContext& context, GameResult result);
        ~FormResult();

    private:
        void onUpdate(float delta) override;

        void onSizeChanged() override;

        void onRestart(Button* btn);
        void onLeave(Button* btn);

        void initUI();

    private:
        GameResult _result = GameResult::Failed;
    };
}