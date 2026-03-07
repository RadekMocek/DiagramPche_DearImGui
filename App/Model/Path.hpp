#pragma once

#include <string>
#include <vector>

#include "imgui.h"

#include "Pathpoint.hpp"
#include "Point.hpp"
#include "../Helper/DrawLayer.hpp"

using ColorTuple = std::tuple<unsigned char, unsigned char, unsigned char, unsigned char>;

struct Path
{
    Point start{};
    std::vector<Point> ends{};
    std::vector<Pathpoint> pathpoints{};

    int shift_start{};
    int shift_end{};

    ColorTuple color = {0, 0, 0, 255};

    bool do_start_arrow = false;
    bool do_end_arrow = true;

    int z = DL_USER_CHANNEL_DEFAULT_PATH;

    std::string label_value{};
    int label_point{};
    int label_shift{};

    //
    [[nodiscard]] constexpr ImVec2 GetShiftVector(const Pivot pivot, const float zoom_level, const bool is_start) const
    {
        const auto sf = static_cast<float>((is_start) ? shift_start : shift_end) * zoom_level;
        switch (pivot) {
        default:
            // Unreachable (?), fallthrough
        case PIVOT_CENTER:
            return {0, 0};
        case PIVOT_TOPLEFT:
            return {-sf, -sf};
        case PIVOT_TOP:
            return {0, -sf};
        case PIVOT_TOPRIGHT:
            return {+sf, -sf};
        case PIVOT_RIGHT:
            return {+sf, 0};
        case PIVOT_BOTTOMRIGHT:
            return {+sf, +sf};
        case PIVOT_BOTTOM:
            return {0, +sf};
        case PIVOT_BOTTOMLEFT:
            return {-sf, +sf};
        case PIVOT_LEFT:
            return {-sf, 0};
        }
    }
};
