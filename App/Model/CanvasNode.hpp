#pragma once

#include "imgui.h"

#include "Pivot.hpp"

struct CanvasNode
{
    ImVec2 top_left;
    ImVec2 bottom_right;
    ImVec2 center;

    // = Canvas interaction =

    // Another metric to determine node's z-value. This one is used while interacting with nodes through the canvas.
    // If more nodes are on top of each other on the z-axis, we need to determine one, that will be chosen e.g. on click.
    // Preferably the one that was drawn last in the imdrawlist channel with biggest layer.
    // With this number, bigger means better ("closer" to the cursor).
    int z_mul;

    int def_line_num{};

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
