#include "form_main.h"
#include "form_scenes.h"
#include "form_config.h"
#include "form_start.h"

#include "game/scene/object_factory.h"
#include "game/scene/game_camera.h"
#include "game/scene/game_scene.h"


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

    _cardGroup = getWidget<CardGroup>("card_group");
    assert(_cardGroup);

    ui::GuiManager::inst().on_drop.connect(this, &FormMain::onDropCard);

#if 0
    auto& roleCfgs = _context.objectFactory().getAllRoleCfgIds();
    for (auto& cfgid : roleCfgs)
    {
        _context.dataCenter().addHandCard(cfgid);
    }
#endif

    auto& handCards = _context.dataCenter().getHandCards();
    for(auto& cfg : handCards)
    {
        auto& props = _context.objectFactory().getObjectCfgProperties(cfg);
        _cardGroup->addCard(props);
    }
}

FormMain::~FormMain()
{
}

void FormMain::onUpdate(float delta)
{
}

void FormMain::onDropCard(ui::GuiManager::DraggingPtr ptr)
{   
    if(!ptr || !ptr->dst_group)
    {
        SPDLOG_INFO("FormMain::onDropCard dragging data error.");
        return;
    }

    auto wgt_name = ptr->dst_group->name();
    auto wgtScene = getWidget<Widget>(wgt_name);
    if (!wgtScene)
    {
        SPDLOG_INFO("FormMain::onDropCard not on form_main.");
        return;
    }

    auto pos = _context.camera().screenToWorld(ptr->drop_screen_pos);
    SPDLOG_INFO("FormMain::onDropCard at ({},{})", pos.x, pos.y);

    auto card = dynamic_cast<CardWidget*>(ptr->widget.get());
    auto cardGroup = dynamic_cast<CardGroup*>(ptr->src_group);
    if(!card || !cardGroup)
    {
        SPDLOG_ERROR("FormMain::onDropCard: drop item is NOT card.");
        return;
    }

    auto cfgid = card->getCfgid();
    auto ent = _context.scene().createActor(cfgid, pos);
    if (!_context.registry().valid(ent))
    {
        SPDLOG_ERROR("FormMain::onDropCard: create actor ({}) failed", cfgid);
        return;
    }

    auto pComm = _context.registry().try_get<CompComm>(ent);
    if (pComm)
    {
        pComm->side = CampSide::Gangster;
    }

    _context.dataCenter().removeHandCard(cfgid);

    //int index = card->getData<int>("index");
    //cardGroup->addWidget(ptr->widget, index);
}

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

    ui::GuiManager::inst().createForm<FormStart>("form_start", _context);
}

void FormMain::onShowScenes(Button* btn)
{
    ui::GuiManager::inst().createForm<FormScenes>("form_scenes", _context);
}


}