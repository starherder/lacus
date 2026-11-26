#include "form_cards.h"
#include "game/scene/object_manager.h"
#include "game/scene/game_data.h"

#include <algorithm>

namespace game
{
    const char* FormCards::rankMarkerName(RoleRank rank)
    {
        switch (rank)
        {
        case RoleRank::Chief:
            return "lbl_rank_chief";
        case RoleRank::Adviser:
            return "lbl_rank_adviser";
        default:
            return "";
        }
    }

    RoleRank FormCards::markerRank(Label* marker)
    {
        if (!marker)
        {
            return RoleRank::Soldier;
        }

        if (marker->name() == rankMarkerName(RoleRank::Chief))
        {
            return RoleRank::Chief;
        }
        if (marker->name() == rankMarkerName(RoleRank::Adviser))
        {
            return RoleRank::Adviser;
        }

        return RoleRank::Soldier;
    }

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
            btnConfirm->on_click.connect(this, &FormCards::onConfirm);
        }

        _gridGroup = getWidget<GridGroup>("grid_cards");
        assert(_gridGroup);

        _cardGroup = getWidget<CardGroup>("cards_all");
        assert(_cardGroup);

        _toolsGroup = getWidget<Group>("hlay_tools");
        assert(_toolsGroup);

	    ui::GuiManager::inst().on_drop.connect(this, &FormCards::onDropCard);

        loadCards();
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

    void FormCards::loadCards()
    {
        auto& objectManager = _context.objectManager();
        const auto& handCards = _context.gameData().getHandCards();

        for (const auto& cfg : objectManager.getAllRoleCfgIds())
        {
            if (!handCards.contains(cfg))
            {
                _cardGroup->addCard(objectManager.getObjectCfgProperties(cfg));
            }
        }

        for (const auto& [cfg, handCard] : handCards)
        {
            if (!objectManager.findObjectCfg(cfg))
            {
                LogWarn("FormCards::loadCards selected card config ({}) not found.", cfg);
                continue;
            }

            auto card = _cardGroup->addCard(objectManager.getObjectCfgProperties(cfg));
            _gridGroup->addWidget(_cardGroup->moveOut(card));
            if (auto marker = takeRankMarker(handCard.rank))
            {
                attachRankMarker(card, marker);
            }
        }

        _cardGroup->setOverlap(_cardGroup->isOverlap());
    }

    void FormCards::onConfirm(Button* btn)
    {
        CardMap handCards;
        for (const auto& widget : _gridGroup->children())
        {
            auto card = dynamic_cast<CardWidget*>(widget.get());
            if (!card)
            {
                continue;
            }

            auto rank = RoleRank::Soldier;
            if (card->getChild<Label>(rankMarkerName(RoleRank::Chief)))
            {
                rank = RoleRank::Chief;
            }
            else if (card->getChild<Label>(rankMarkerName(RoleRank::Adviser)))
            {
                rank = RoleRank::Adviser;
            }

            auto cfgid = card->getCfgid();
            handCards.emplace(cfgid, HandCard{ cfgid, rank });
        }

        _context.gameData().setHandCards(std::move(handCards));
        onHandCardsConfirmed();
    }

    void FormCards::onDropCard(ui::GuiManager::DraggingPtr ptr)
    {
        if (!ptr || !ptr->dst_group)
        {
            LogInfo("FormCards::onDropCard dragging data error.");
            return;
        }

        if (onDropRankMarker(ptr))
        {
            return;
        }

        if (ptr->src_group != _gridGroup && ptr->src_group != _cardGroup)
        {
            return;
        }

        auto card = dynamic_cast<CardWidget*>(ptr->widget.get());
        if(!card)
        {
            LogError("FormCards::onDropCard: drop item is NOT card.");
            return;
        }

        if (ptr->dst_group == _gridGroup)
        {
            _gridGroup->addWidget(ptr->widget);
            return;
        }

        if (ptr->src_group == _gridGroup && ptr->dst_group == _cardGroup)
        {
            returnCardRankMarkers(card);
            _cardGroup->addWidget(ptr->widget);
            return;
        }

        auto srcGroup = dynamic_cast<Group*>(ptr->src_group);
        if (srcGroup)
        {
            srcGroup->addWidget(ptr->widget);
        }
    }

    bool FormCards::onDropRankMarker(ui::GuiManager::DraggingPtr ptr)
    {
        auto marker = dynamic_cast<Label*>(ptr->widget.get());
        if (markerRank(marker) == RoleRank::Soldier)
        {
            return false;
        }

        auto sourceCard = dynamic_cast<CardWidget*>(ptr->src_group);
        if (ptr->src_group != _toolsGroup && (!sourceCard || sourceCard->parent() != _gridGroup))
        {
            return false;
        }

        auto targetCard = ptr->dst_group == _gridGroup ? findGridCardAt(ptr->drop_screen_pos) : nullptr;
        if (targetCard)
        {
            attachRankMarker(targetCard, ptr->widget);
        }
        else if (sourceCard && ptr->dst_group != _toolsGroup)
        {
            attachRankMarker(sourceCard, ptr->widget);
        }
        else
        {
            returnRankMarker(ptr->widget);
        }
        return true;
    }

    void FormCards::attachRankMarker(ui::CardWidget* card, ui::WidgetPtr marker)
    {
        if (!card || !marker)
        {
            return;
        }

        returnCardRankMarkers(card);

        marker->setScaleInGroup(false);
        marker->setSize({ 72.0f, 28.0f });
        marker->setPos({ std::max(0.0f, card->size().x - marker->size().x - 8.0f), 8.0f });
        card->addWidget(marker);
        card->bringTop(marker.get());
    }

    ui::WidgetPtr FormCards::takeRankMarker(RoleRank rank)
    {
        if (!_toolsGroup)
        {
            return nullptr;
        }

        auto markerName = rankMarkerName(rank);
        if (markerName[0] == '\0')
        {
            return nullptr;
        }

        auto marker = _toolsGroup->getChild<Label>(markerName);
        return marker ? _toolsGroup->moveOut(marker) : nullptr;
    }

    void FormCards::returnRankMarker(ui::WidgetPtr marker)
    {
        if (!_toolsGroup || !marker)
        {
            return;
        }

        marker->setCanDragOut(true);
        marker->setAcceptEvent(true);
        marker->setScaleInGroup(true);
        _toolsGroup->addWidget(marker);
    }

    void FormCards::returnCardRankMarkers(ui::CardWidget* card)
    {
        if (!card)
        {
            return;
        }

        for (auto rankName : { rankMarkerName(RoleRank::Chief), rankMarkerName(RoleRank::Adviser) })
        {
            auto marker = card->getChild<Label>(rankName);
            if (!marker)
            {
                continue;
            }

            returnRankMarker(card->moveOut(marker));
        }
    }

    ui::CardWidget* FormCards::findGridCardAt(const Vec2& pos)
    {
        if (!_gridGroup)
        {
            return nullptr;
        }

        const auto& children = _gridGroup->children();
        for (auto it = children.rbegin(); it != children.rend(); ++it)
        {
            auto card = dynamic_cast<CardWidget*>(it->get());
            if (card && card->getAbsRect().contains(pos))
            {
                return card;
            }
        }

        return nullptr;
    }
}
