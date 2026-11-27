#include "form_cards.h"
#include "game/scene/object_manager.h"
#include "game/scene/game_data.h"
#include "game/scene/save_manager.h"
#include "game/ui/form_tip.h"
#include "game/ui/form_utils.h"
#include "utility/translator.h"

#include <utility>

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

    int FormCards::rankMarkerToolIndex(RoleRank rank)
    {
        switch (rank)
        {
        case RoleRank::Chief:
            return 1;
        case RoleRank::Adviser:
            return 2;
        default:
            return -1;
        }
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
        _gridGroup->clear();
        _cardGroup->clear();

        auto& objectManager = _context.objectManager();
        auto& gameData = _context.gameData();
        const auto& handCards = _context.gameData().getHandCards();

        for (const auto& cfg : objectManager.getAllRoleCfgIds())
        {
            if (gameData.isCardUnlocked(cfg) && !handCards.contains(cfg))
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
            card->setData("rank", static_cast<int>(handCard.rank));
            clearSameRankFromOtherCards(card, handCard.rank);
            FormUtils::addRankMarker(card, handCard.rank);
        }

        _cardGroup->setOverlap(_cardGroup->isOverlap());
    }

    void FormCards::onConfirm(Button* btn)
    {
        CardMap handCards;
        bool hasChief = false;

        for (const auto& widget : _gridGroup->children())
        {
            auto card = dynamic_cast<CardWidget*>(widget.get());
            if (!card)
            {
                continue;
            }

            auto rank = static_cast<RoleRank>(card->getData<int>("rank"));
            if (rank == RoleRank::Chief)
            {
                hasChief = true;
            }

            auto cfgid = card->getCfgid();
            handCards.emplace(cfgid, HandCard{ cfgid, rank });
        }

        if (!hasChief)
        {
            auto tip = ui::GuiManager::inst().showForm<FormTip>("form_tip", _context);
            tip->setTipMode(TipMode::Ok);
            tip->setText(Trans("ui_tip_no_chief"));
            return;
        }

        _context.gameData().setHandCards(std::move(handCards));
        SaveManager::inst().saveGameData(_context.resPath() / "save/game_data.json", _context.gameData());
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
            _gridGroup->addWidget(ptr->widget, _gridGroup->getInsertIndexAt(ptr->drop_screen_pos));
            if (ptr->src_group != _gridGroup)
            {
                card->setData("rank", static_cast<int>(RoleRank::Soldier));
                FormUtils::clearRankMarker(card);
            }
            return;
        }

        if (ptr->src_group == _gridGroup && ptr->dst_group == _cardGroup)
        {
            returnCardRankMarkers(card);
            card->setData("rank", static_cast<int>(RoleRank::Soldier));
            _cardGroup->addWidget(ptr->widget, _cardGroup->getInsertIndexAt(ptr->drop_screen_pos));
            return;
        }

        if (ptr->src_group == _cardGroup && ptr->dst_group == _cardGroup)
        {
            _cardGroup->addWidget(ptr->widget, _cardGroup->getInsertIndexAt(ptr->drop_screen_pos));
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

        auto rank = markerRank(dynamic_cast<Label*>(marker.get()));
        clearSameRankFromOtherCards(card, rank);
        card->setData("rank", static_cast<int>(rank));
        FormUtils::addRankMarker(card, rank);
        returnRankMarker(marker);
    }

    void FormCards::clearSameRankFromOtherCards(ui::CardWidget* keepCard, RoleRank rank)
    {
        if (!_gridGroup || rank == RoleRank::Soldier)
        {
            return;
        }

        for (auto& widget : _gridGroup->children())
        {
            auto card = dynamic_cast<CardWidget*>(widget.get());
            if (!card || card == keepCard)
            {
                continue;
            }

            if (static_cast<RoleRank>(card->getData<int>("rank")) == rank)
            {
                card->setData("rank", static_cast<int>(RoleRank::Soldier));
                FormUtils::clearRankMarker(card);
            }
        }
    }

    void FormCards::returnRankMarker(ui::WidgetPtr marker)
    {
        if (!_toolsGroup || !marker)
        {
            return;
        }

        auto rank = markerRank(dynamic_cast<Label*>(marker.get()));
        marker->setCanDragOut(true);
        marker->setAcceptEvent(true);
        _toolsGroup->addWidget(marker, rankMarkerToolIndex(rank));
        marker->setScaleInGroup(false);
        marker->setSize({ 64.0f, 39.0f });
    }

    void FormCards::returnCardRankMarkers(ui::CardWidget* card)
    {
        if (!card)
        {
            return;
        }

        FormUtils::clearRankMarker(card);
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
