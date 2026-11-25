#include "form_cards.h"
#include "game/scene/object_manager.h"
#include "game/scene/game_data.h"

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
        auto btnConfirm = getWidget<Button>("btn_confirm");
        if (btnConfirm)
        {
            btnConfirm->on_click.connect([this](Button* btn) { onCardGroupConfirm(); });
        }

        _gridGroup = getWidget<GridGroup>("grid_cards");
        assert(_gridGroup);

        _cardGroup = getWidget<CardGroup>("cards_all");
        assert(_cardGroup);

	    ui::GuiManager::inst().on_drop.connect(this, &FormCards::onDropCard);

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
        auto selectedCards = _context.gameData().getCardGroup();

        auto moveCardToGrid = [this](CardWidget* card)
        {
            if (!card)
            {
                return;
            }

            auto widget = _cardGroup->moveOut(card);
            if (widget)
            {
                _gridGroup->addWidget(widget);
            }
        };

        auto& roleCfgs = _context.objectManager().getAllRoleCfgIds();
        for (auto& cfg : roleCfgs)
        {
            auto& props = _context.objectManager().getObjectCfgProperties(cfg);
            auto card = _cardGroup->addCard(props);

            auto selected = selectedCards.find(cfg);
            if (selected != selectedCards.end())
            {
                selectedCards.erase(selected);
                moveCardToGrid(card);
            }
        }

        for (const auto& cfg : selectedCards)
        {
            if (!_context.objectManager().findObjectCfg(cfg))
            {
                LogWarn("FormCards::loadCars selected card config ({}) not found.", cfg);
                continue;
            }

            auto& props = _context.objectManager().getObjectCfgProperties(cfg);
            auto card = _cardGroup->addCard(props);
            moveCardToGrid(card);
        }

        _cardGroup->setOverlap(_cardGroup->isOverlap());
    }

    void FormCards::onDropCard(ui::GuiManager::DraggingPtr ptr)
    {
        if (!ptr || !ptr->dst_group)
        {
            LogInfo("FormCards::onDropCard dragging data error.");
            return;
        }

        if (ptr->dst_group == _gridGroup)
        {
            _gridGroup->addWidget(ptr->widget);
        }

        auto card = dynamic_cast<CardWidget*>(ptr->widget.get());
        if(!card)
        {
            LogError("FormCards::onDropCard: drop item is NOT card.");
            return;
        }

        auto cfgid = card->getCfgid();
        _context.gameData().addToCardGroup(cfgid);

        // remove from hand card group
        if (ptr->src_group == _gridGroup && ptr->dst_group != _gridGroup)
        {
            _gridGroup->removeChild(card->name());
            _context.gameData().removeFromCardGroup(card->getCfgid());
            return;
        }
    }
}
