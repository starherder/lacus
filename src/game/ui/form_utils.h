#pragma once

#include "game/ecs/comm_comp.h"

namespace ui {
    class Group;
}

namespace game
{
    class FormUtils
    {
    public:
        static void addRankMarker(ui::Group* group, RoleRank rank);
        static void clearRankMarker(ui::Group* group);

    private:
        static constexpr const char* RankMarkerName = "__rank_marker__";
        static constexpr float MarkerPadding = 6.0f;

        static const char* rankMarkerTile(RoleRank rank);
    };
}
