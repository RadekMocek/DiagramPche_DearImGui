#pragma once

#include "imgui.h"

#include "Pivot.hpp"

struct CanvasNode
{
    ImVec2 top_left;
    ImVec2 bottom_right;
    ImVec2 center;

    [[nodiscard]] constexpr ImVec2 GetExactPointFromPivot(const Pivot pivot) const
    {
        switch (pivot) {
        default:
            // Unreachable (?), fallthrough
        case PIVOT_TOPLEFT:
            return top_left;
        case PIVOT_TOP:
            return {center.x, top_left.y};
        case PIVOT_TOPRIGHT:
            return {bottom_right.x, top_left.y};
        case PIVOT_RIGHT:
            return {bottom_right.x, center.y};
        case PIVOT_BOTTOMRIGHT:
            return bottom_right;
        case PIVOT_BOTTOM:
            return {center.x, bottom_right.y};
        case PIVOT_BOTTOMLEFT:
            return {top_left.x, bottom_right.y};
        case PIVOT_LEFT:
            return {top_left.x, center.y};
        case PIVOT_CENTER:
            return center;
        }
    }

    [[nodiscard]] constexpr bool IsPointInsideIncl(const ImVec2 point) const
    {
        return point.x >= top_left.x && point.x <= bottom_right.x && point.y >= top_left.y && point.y <= bottom_right.y;
    }
};
