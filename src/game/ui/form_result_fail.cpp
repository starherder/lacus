#include "form_result_fail.h"


namespace game
{

    FormResultFail::FormResultFail(const std::string& name, GameContext& context) : FormLogicBase(name, context)
    {
        load(_context.resPath() / "ui/form_result_fail.xml");

    }

    FormResultFail::~FormResultFail()
    {
    }

    void FormResultFail::onUpdate(float delta)
    {
    }

    void FormResultFail::onSizeChanged()
    {
    }
}