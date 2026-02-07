#pragma once

#include <vector>

#include "imgui.h"

#include "Pathpoint.hpp"
#include "Point.hpp"

struct Path
{
    Point start{};
    std::vector<Point> ends{};
    std::vector<Pathpoint> pathpoints{};

    int shift{};

    std::tuple<unsigned char, unsigned char, unsigned char, unsigned char> color = {0, 0, 0, 255};

    bool do_start_arrow = false;
    bool do_end_arrow = true;

    int z = DRAW_LIST_CHANNEL_DEFAULT_PATH;

    //
    [[nodiscard]] constexpr ImVec2 GetShiftVector(const Pivot pivot, const float zoom_level) const
    {
        const auto sf = static_cast<float>(shift) * zoom_level;
        switch (pivot) {
        default: // UNKNOWN_PIVOT || CENTER
            return {0, 0};
        case TOPLEFT:
            return {-sf, -sf};
        case TOP:
            return {0, -sf};
        case TOPRIGHT:
            return {+sf, -sf};
        case RIGHT:
            return {+sf, 0};
        case BOTTOMRIGHT:
            return {+sf, +sf};
        case BOTTOM:
            return {0, +sf};
        case BOTTOMLEFT:
            return {-sf, +sf};
        case LEFT:
            return {-sf, 0};
        }
    }
};
