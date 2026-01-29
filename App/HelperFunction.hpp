#pragma once

#include <cmath>
#include <tuple>

#include "imgui.h"

// == Dear ImGui helper functions ==
// = ImVec2 =
// Curiosity: MSVC says this cannot be constexpr, MinGW is OK with it
constexpr ImVec2 ImVec2Normalized(const ImVec2 vec)
{
    const auto magnitude = sqrtf((vec.x * vec.x) + (vec.y * vec.y));
    return {vec.x / magnitude, vec.y / magnitude};
}

constexpr ImVec2 ImVec2Orthogonalized(const ImVec2 vec)
{
    return {-vec.y, vec.x};
}

// = Color =
constexpr ImU32 GetColorFromTuple(const std::tuple<unsigned char, unsigned char, unsigned char, unsigned char>& tup)
{
    return IM_COL32(std::get<0>(tup), std::get<1>(tup), std::get<2>(tup), std::get<3>(tup));
}

// = Draw =
inline void DrawArrowTip(
    ImDrawList* draw_list,
    const ImVec2 p1,
    const ImVec2 p2,
    const float zoom_level,
    const ImU32 color
)
{
    // TODO magic numbers
    const auto p2_to_p1 = ImVec2Normalized(p1 - p2);
    const auto point_slightly_before_p2 = p2 + p2_to_p1 * 12 * zoom_level;
    const auto p2_orthogonal_addition = ImVec2Orthogonalized(p2_to_p1) * 4 * zoom_level;
    draw_list->AddTriangleFilled(p2,
                                 point_slightly_before_p2 - p2_orthogonal_addition,
                                 point_slightly_before_p2 + p2_orthogonal_addition,
                                 color);
}

// == A huge time saver :) ==
#include <iostream>
#define print(x) std::cout << x << '\n'
#define printerr(x) std::cerr << "[ERR] " << x << '\n'
#define printvec(v) std::cout << "[" << v.x << ", " << v.y << "]\n"
