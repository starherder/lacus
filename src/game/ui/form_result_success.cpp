#include "form_result_success.h"


namespace game
{

    FormResultSuccess::FormResultSuccess(const std::string& name, GameContext& context) : FormLogicBase(name, context)
    {
        load(_context.resPath() / "ui/form_result_fail.xml");

    }

    FormResultSuccess::~FormResultSuccess()
    {
    }

    void FormResultSuccess::onUpdate(float delta)
    {
    }

    void FormResultSuccess::onSizeChanged()
    {
    }
}