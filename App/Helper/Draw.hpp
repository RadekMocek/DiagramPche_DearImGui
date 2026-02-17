#pragma once

#include <cmath>

#include "imgui.h"

#include "../Config.hpp"

inline ImVec2 ImVec2Normalized(const ImVec2 vec)
{
    const auto magnitude = sqrtf((vec.x * vec.x) + (vec.y * vec.y));
    return {vec.x / magnitude, vec.y / magnitude};
}

constexpr ImVec2 ImVec2Orthogonalized(const ImVec2 vec)
{
    return {-vec.y, vec.x};
}

inline void DrawArrowTip(
    ImDrawList* draw_list,
    const ImVec2 p1,
    const ImVec2 p2,
    const float zoom_level,
    const ImU32 color
)
{
    const auto p2_to_p1 = ImVec2Normalized(p1 - p2);
    const auto point_slightly_before_p2 = p2 + p2_to_p1 * TIP_ARROW_LENGTH * zoom_level;
    const auto p2_orthogonal_addition = ImVec2Orthogonalized(p2_to_p1) * TIP_ARROW_SPAN * zoom_level;
    draw_list->AddTriangleFilled(p2,
                                 point_slightly_before_p2 - p2_orthogonal_addition,
                                 point_slightly_before_p2 + p2_orthogonal_addition,
                                 color);
}
