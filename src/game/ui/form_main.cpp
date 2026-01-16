#include "form_main.h"
#include "form_scenes.h"

namespace game 
{

FormMain::FormMain(const std::string& name, GameContext& context) : FormLogicBase(name, context)
{
    setMaximize(true);

    root()->setDragable(false);

    root()->setNoEvent(true);

    root()->setBgColor({ 0,0,0, 50 });

    _btnScenes = root()->createChild<ui::Button>("btn_scenes");
    _btnScenes->setPos({size().x-150, 50});
    _btnScenes->setSize({100, 50});
    _btnScenes->setText("scenes");
    _btnScenes->on_click.connect([this](ui::Button* btn) {
        ui::GuiManager::inst().showForm<FormScenes>("form_scenes", _context);
    });
}

FormMain::~FormMain()
{
}

void FormMain::onUpdate(float delta)
{
    _btnScenes->setPos({ size().x - 150, 50 });
}

}