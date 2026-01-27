#include "form_scenes.h"
#include "game/ui/ui_logic_events.h"
#include "form_entry.h"
#include "ui/gui_manager.h"


namespace game 
{

FormScenes::FormScenes(const std::string& name, GameContext& context) : FormLogicBase(name, context)
{
    load(_context.resPath() / "ui/form_scenes.xml");

    auto group = getWidget<ExpandGroup>("exgroup_scenes");
    if(group)
    {
        group->setPos({0, 0});
        group->setSize(size());
    }

    auto btnClose = getWidget<ui::Button>("btn_close");
    if(btnClose)
    {
        btnClose->setPos({size().x-100, 50});
        btnClose->on_click.connect([this](ui::Button* btn) { close(); });
    }

    auto exgroup = getWidget<ExpandGroup>("exgroup_scenes");
    if(exgroup)
    {
        auto scenebtns = exgroup->items();
        for(auto& ctrl : scenebtns) 
        {
            if(auto btn = dynamic_cast<Button*>(ctrl)) 
            {
                btn->on_click.connect(this, &FormScenes::onSelectScene);
            }
        }
    }
}

FormScenes::~FormScenes()
{
}

void FormScenes::onUpdate(float delta)
{
}

void FormScenes::onSizeChanged()
{
    auto group = getWidget<ExpandGroup>("exgroup_scenes");
    if (group)
    {
        group->setPos({ 0, 0 });
        group->setSize(size());
    }

    auto btnClose = getWidget<ui::Button>("btn_close");
    if (btnClose)
    {
        btnClose->setPos({ size().x - 100, 50 });
    }
}

void FormScenes::onSelectScene(ui::Button* btn)
{
    auto name = btn->getData<std::string>("scene");

    auto form = ui::GuiManager::inst().createForm<FormEntry>("form_entry", _context);
    if (form)
    {
        form->selectScene(name);
    }
}
}