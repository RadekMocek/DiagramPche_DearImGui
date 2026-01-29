#pragma once

// == Dear ImGui ImVec helper functions ==
#include "imgui.h"

constexpr ImVec2 ImVec2Sum(const ImVec2 v1, const ImVec2 v2)
{
    return {v1.x + v2.x, v1.y + v2.y};
}

/*
constexpr ImVec2 ImVec2TranslationXY(const ImVec2 v, const float c)
{
    return {v.x + c, v.y + c};
}

constexpr ImVec2 ImVec2TranslationX(const ImVec2 v, const float c)
{
    return {v.x + c, v.y};
}

constexpr ImVec2 ImVec2TranslationY(const ImVec2 v, const float c)
{
    return {v.x, v.y + c};
}
/**/

// == A huge time saver :) ==
#include <iostream>
#define print(x) std::cout << x << '\n'
#define printerr(x) std::cerr << "[ERR] " << x << '\n'
