#include "form_cards.h"
#include "game/scene/object_factory.h"

namespace game
{

    FormCards::FormCards(const std::string& name, GameContext& context) : FormLogicBase(name, context)
    {
        load(_context.resPath() / "ui/form_cards.xml");

        auto btnClose = getWidget<Button>("btn_close");
        if (btnClose)
        {
            btnClose->on_click.connect([this](Button* btn) { close(); });
        }

        _cardGroup = getWidget<CardGroup>("card_group");
        assert(_cardGroup);

        loadCars();
    }

    FormCards::~FormCards()
    {
    }

    void FormCards::onUpdate(float delta)
    {
    }

    void FormCards::onSizeChanged()
    {
    }

    void FormCards::loadCars()
    {
        auto& roleCfgs = _context.objectFactory().getAllRoleCfgIds();
        for (auto& cfg : roleCfgs)
        {
            auto& props = _context.objectFactory().getObjectCfgProperties(cfg);
            _cardGroup->addCard(props);
        }
    }
}