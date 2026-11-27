#include "form_main.h"
#include "form_bag.h"
#include "form_scenes.h"
#include "form_config.h"
#include "form_start.h"

#include "game/scene/object_manager.h"
#include "game/scene/game_camera.h"
#include "game/scene/game_data.h"
#include "game/scene/game_scene.h"
#include "game/ui/form_utils.h"
#include "game/ui/form_scenes.h"
#include "game/ui/ui_logic_events.h"

namespace game 
{

FormMain::FormMain(const std::string& name, GameContext& context) : FormLogicBase(name, context)
{
    load(_context.resPath() / "ui/form_main.xml");

    auto btnClose = getWidget<Button>("btn_close");
    if (btnClose)
    {
        btnClose->on_click.connect(this, &FormMain::onLeaveScene);
    }

    auto btnScenes = getWidget<Button>("btn_scenes");
    if (btnScenes)
    {
        btnScenes->on_click.connect(this, &FormMain::onShowScenes);
    }

    auto btnConfig = getWidget<Button>("btn_config");
    if (btnConfig)
    {
        btnConfig->on_click.connect(this, &FormMain::onShowConfig);
    }

    auto btnBag = getWidget<Button>("btn_bag");
    if (btnBag)
    {
        btnBag->on_click.connect(this, &FormMain::onShowBag);
    }

/*
    ui::GuiManager::inst().on_drop.connect(this, &FormMain::onDropCard);

    _cardGroup = getWidget<CardGroup>("card_group");
    assert(_cardGroup);

    for (const auto& [cfg, handCard] : _context.gameData().getHandCards())
    {
        auto& props = _context.objectManager().getObjectCfgProperties(cfg);
        auto card = _cardGroup->addCard(props);
        FormUtils::addRankMarker(card, handCard.rank);
    }

    _cardGroup->setVisible(false);
*/
}

FormMain::~FormMain()
{
}

void FormMain::onUpdate(float delta)
{
}

/*
void FormMain::onDropCard(ui::GuiManager::DraggingPtr ptr)
{   
    if (!ptr || !ptr->dst_group || ptr->src_group != _cardGroup)
    {
        return;
    }

    auto index = ptr->widget->getData<int>("index");
    if (ptr->dst_group != getWidget<Widget>("wgt_scene"))
    {
        index = ptr->dst_group == _cardGroup ? _cardGroup->getInsertIndexAt(ptr->drop_screen_pos) : index;
        _cardGroup->addWidget(ptr->widget, index);
        return;
    }

    auto card = dynamic_cast<CardWidget*>(ptr->widget.get());
    if (!card)
    {
        LogError("FormMain::onDropCard: drop item is NOT card.");
        return;
    }

    auto cfgid = card->getCfgid();
    auto handCard = _context.gameData().getHandCard(cfgid);
    if (!handCard)
    {
        return;
    }

    auto rank = handCard->rank;
    auto screenPos = ui::GuiManager::inst().uiToScreenPoint(ptr->drop_screen_pos);
    auto worldPos = _context.camera().screenToWorld(screenPos);
    auto grid = _context.scene().getGridFromPos(worldPos);
    auto pos = _context.scene().getGridCenterPos(grid);
    LogInfo("FormMain::onDropCard at grid ({},{}), pos ({},{})", grid.x, grid.y, pos.x, pos.y);

    auto ent = _context.scene().createObjectInScene(cfgid, pos, CampSide::Gangster);
    if (!_context.registry().valid(ent))
    {
        LogError("FormMain::onDropCard: create actor ({}) failed", cfgid);
        _cardGroup->addWidget(ptr->widget, index);
        return;
    }

    auto pComm = _context.registry().try_get<CompComm>(ent);
    if (pComm)
    {
        pComm->rank = rank;
    }

    _context.gameData().removeHandCard(cfgid);
}
*/

void FormMain::onSizeChanged()
{
}

void FormMain::onWindowResized(const Vec2& sz)
{
    FormLogicBase::onWindowResized(sz);

}

void FormMain::onLeaveScene(Button* btn)
{
    _context.scene().unload();

    ui::GuiManager::inst().showForm<FormStart>("form_start", _context);
}

void FormMain::onShowScenes(Button* btn)
{
    ui::GuiManager::inst().showForm<FormScenes>("form_scenes", _context);
}

void FormMain::onShowConfig(Button* btn)
{
    ui::GuiManager::inst().showForm<FormConfig>("form_config", _context);
}

void FormMain::onShowBag(Button* btn)
{
    auto formBag = ui::GuiManager::inst().showForm<FormBag>("form_bag", _context);
    if (formBag)
    {
        formBag->refreshBag();
    }
}


}
