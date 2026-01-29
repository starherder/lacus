#include "form_result.h"
#include "utility/translator.h"

namespace game
{

    FormResult::FormResult(const std::string& name, GameContext& context, GameResult result) 
        : FormLogicBase(name, context), _result(result)
    {
        load(_context.resPath() / "ui/form_result.xml");

        auto btnRestart = getWidget<Button>("btn_restart");
        if (btnRestart)
        {
            btnRestart->on_click.connect(this, &FormResult::onRestart);
        }

        auto btnLeave = getWidget<Button>("btn_leave");
        if (btnLeave)
        {
            btnLeave->on_click.connect(this, &FormResult::onLeave);
        }

        initUI();
    }

    FormResult::~FormResult()
    {
    }

    void FormResult::initUI()
    {
        auto lbl_result = getWidget<Label>("lbl_result");
        if (lbl_result)
        {
            auto txt = (_result == GameResult::Success) ? Trans("SUCCESS") : Trans("FAILED");
            auto col = (_result == GameResult::Success) ? Color::Red : Color::Gray;

            lbl_result->setText(txt);
            lbl_result->setTextColor(col);
        }
    }

    void FormResult::onUpdate(float delta)
    {
    }

    void FormResult::onSizeChanged()
    {
    }

    void FormResult::onRestart(Button* btn)
    {

    }

    void FormResult::onLeave(Button* btn)
    {

    }
}