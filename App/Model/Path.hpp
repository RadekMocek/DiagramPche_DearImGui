#pragma once

#include <vector>

#include "imgui.h"

#include "Pathpoint.hpp"
#include "Point.hpp"

struct Path
{
    Point start{};
    std::vector<Point> ends{};
    std::vector<Pathpoint> path_points{};

    int shift{};

    [[nodiscard]] constexpr ImVec2 GetShiftDirection(const Pivot pivot, const float zoom_level) const
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
