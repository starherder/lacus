#include "form_utils.h"

#include "ui/ui.h"

#include <algorithm>

namespace game
{
    const char* FormUtils::rankMarkerTile(RoleRank rank)
    {
        switch (rank)
        {
        case RoleRank::Chief:
            return "ui:rank_chief";
        case RoleRank::Adviser:
            return "ui:rank_advisor";
        default:
            return "";
        }
    }

    void FormUtils::addRankMarker(ui::Group* group, RoleRank rank)
    {
        if (!group)
        {
            return;
        }

        clearRankMarker(group);

        auto tile = rankMarkerTile(rank);
        if (!tile || tile[0] == '\0')
        {
            return;
        }

        const Vec2 markerSize = { 64.0f, 39.0f };

        auto marker = group->createChild<ui::Label>(RankMarkerName);
        marker->setText("");
        marker->setTexTile(tile);
        marker->setBgColor(engine::Color::White);
        marker->setBorderColor(engine::Color::Invalid);
        marker->setBorderSize(0.0f);
        marker->setCanDragOut(false);
        marker->setAcceptEvent(false);
        marker->setScaleInGroup(false);
        marker->setSize(markerSize);
        marker->setPos({
            std::max(0.0f, group->size().x - markerSize.x - MarkerPadding),
            MarkerPadding
        });
        group->bringTop(marker);
    }

    void FormUtils::clearRankMarker(ui::Group* group)
    {
        if (!group)
        {
            return;
        }

        group->removeChild(RankMarkerName);
    }
}
