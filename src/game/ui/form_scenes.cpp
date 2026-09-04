#include "form_scenes.h"
#include "game/ui/ui_logic_events.h"
#include "form_entry.h"
#include "ui/gui_manager.h"
#include "form_cards.h"


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
    if (btnClose)
    {
        btnClose->on_click.connect([this](ui::Button* btn) { close(); });
    }

    auto btnCards = getWidget<ui::Button>("btn_cards");
    if (btnCards)
    {
        btnCards->on_click.connect(this, &FormScenes::onShowCards);
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
        btnClose->setPos({ 20, 20 });
    }

    auto btnCards = getWidget<ui::Button>("btn_cards");
    if (btnCards)
    {
        btnCards->setPos({ size().x - 210, 50 });
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

void FormScenes::onShowCards(ui::Button* btn)
{
    auto formCards = ui::GuiManager::inst().getForm<FormCards>("form_cards");
    if (formCards)
    {
        ui::GuiManager::inst().closeForm("form_cards");
    }
    else
    {
        ui::GuiManager::inst().createForm<FormCards>("form_cards", _context);
    }
}


}