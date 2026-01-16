#include "form_scenes.h"


namespace game 
{

FormScenes::FormScenes(const std::string& name) : Form(name)
{
    setMaximize(true);
    
    root()->setDragable(false);

    auto btnClose = root()->createChild<ui::Button>("btn_close");
    btnClose->on_click.connect(this, &FormScenes::onCloseClick);
}

FormScenes::~FormScenes()
{
}

void FormScenes::onCloseClick(Button* btn)
{
    ui::GuiManager::inst().closeForm("form_scenes");
}

void FormScenes::onUpdate(float delta)
{

}

}