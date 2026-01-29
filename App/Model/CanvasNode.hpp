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
            // Undefined
        case TOPLEFT:
            return top_left;
        case TOP:
            return {center.x, top_left.y};
        case TOPRIGHT:
            return {bottom_right.x, top_left.y};
        case RIGHT:
            return {bottom_right.x, center.y};
        case BOTTOMRIGHT:
            return bottom_right;
        case BOTTOM:
            return {center.x, bottom_right.y};
        case BOTTOMLEFT:
            return {top_left.x, bottom_right.y};
        case LEFT:
            return {top_left.x, center.y};
        case CENTER:
            return center;
        }
    }

    [[nodiscard]] constexpr bool IsPointInsideIncl(const ImVec2 point) const
    {
        return point.x >= top_left.x && point.x <= bottom_right.x && point.y >= top_left.y && point.y <= bottom_right.y;
    }
};
