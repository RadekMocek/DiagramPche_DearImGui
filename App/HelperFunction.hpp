#pragma once

#include <tuple>

#include "imgui.h"

// == Dear ImGui helper functions ==
// = Color =
constexpr ImU32 GetColorFromTuple(const std::tuple<unsigned char, unsigned char, unsigned char, unsigned char>& tup)
{
    return IM_COL32(std::get<0>(tup), std::get<1>(tup), std::get<2>(tup), std::get<3>(tup));
}

// == A huge time saver :) ==
#include <iostream>
#define print(x) std::cout << x << '\n'
#define printerr(x) std::cerr << "[ERR] " << x << '\n'
