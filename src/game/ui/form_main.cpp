#include "form_main.h"
#include "form_scenes.h"

#include "game/scene/object_factory.h"
#include "game/scene/game_camera.h"
#include "game/scene/game_scene.h"


namespace game 
{

FormMain::FormMain(const std::string& name, GameContext& context) : FormLogicBase(name, context)
{
    root()->setMovable(false);

    root()->setAcceptEvent(false);

    root()->setBgColor({ 0,0,0, 0 });

    _btnScenes = root()->createChild<ui::Button>("btn_scenes");
    _btnScenes->setPos({size().x-150, 50});
    _btnScenes->setSize({100, 50});
    _btnScenes->setText("scenes");
    _btnScenes->on_click.connect([this](ui::Button* btn) {
        ui::GuiManager::inst().showForm<FormScenes>("form_scenes", _context);
    });

    _cardGroup = root()->createChild<CardGroup>("card_group");

    ui::GuiManager::inst().on_drop.connect(this, &FormMain::onDropCard);

    auto& objcfgs = _context.objectFactory().getAllObjectCfgIds();
    for(auto& cfg : objcfgs)
    {
        auto& props = _context.objectFactory().getObjectCfgProperties(cfg);
        _cardGroup->addCard(props);
    }

    setMaximize(true);
}

FormMain::~FormMain()
{
}

void FormMain::onUpdate(float delta)
{
}

void FormMain::onDropCard(ui::GuiManager::DraggingPtr ptr)
{    
    if(!ptr)
    {
        spdlog::info("FormMain::onDropCard dragging data error.");
        return;
    }

    auto pos = _context.camera().screenToWorld(ptr->drop_screen_pos);
    spdlog::info("FormMain::onDropCard at ({},{})", pos.x, pos.y);

    auto card = dynamic_cast<CardWidget*>(ptr->widget.get());
    auto cardGroup = dynamic_cast<CardGroup*>(ptr->src_group);
    if(!card || !cardGroup)
    {
        spdlog::error("FormMain::onDropCard: drop item is NOT card.");
        return;
    }

    auto cfgid = card->getCfgid();
    auto ent = _context.currentScene().createActor(cfgid, pos);
    if (!_context.registry().valid(ent))
    {
        spdlog::error("FormMain::onDropCard: create actor ({}) failed", cfgid);
        return;
    }

    auto pComm = _context.registry().try_get<CompComm>(ent);
    if (pComm)
    {
        pComm->side = CampSide::Gangster;
    }

    int index = card->getData<int>("index");
    cardGroup->addWidget(ptr->widget, index);
}

void FormMain::onSizeChanged()
{
    _btnScenes->setPos({ size().x - 150, 50 });

    Vec2 cardGroupSZ = { size().x / 2, 200 };
    Vec2 cardGroupPos = { (size().x - cardGroupSZ.x) / 2, size().y - cardGroupSZ.y };

    _cardGroup->setSize(cardGroupSZ);
    _cardGroup->setPos(cardGroupPos);
    _cardGroup->setOverlap(true);
    
}


}