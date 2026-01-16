#include "form_main.h"


namespace game 
{

FormMain::FormMain(const std::string& name) : Form(name)
{
    setMaximize(true);

    root()->setDragable(false);

    root()->setNoEvent(true);

    root()->setBgColor({ 0,0,0,0 });
}

FormMain::~FormMain()
{
}

void FormMain::onUpdate(float delta)
{

}

}