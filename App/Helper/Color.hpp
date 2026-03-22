#pragma once

#include <tuple>

#include "imgui.h"

using ColorTuple = std::tuple<unsigned char, unsigned char, unsigned char, unsigned char>;

constexpr ImU32 GetImU32FromColorTuple(const ColorTuple& tup)
{
    const auto [r, g, b, a] = tup;
    return IM_COL32(r, g, b, a);
}

constexpr ImVec4 GetImVec4FromColorTuple(const ColorTuple& tup)
{
    const auto [r, g, b, a] = tup;
    return ImVec4(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
}

constexpr ColorTuple GetColorTupleFromString(const std::string& color_str)
{
    if (color_str.length() == 9 && color_str[0] == '#') {
        unsigned int r = 0, g = 0, b = 0, a = 0;
        std::sscanf(color_str.c_str(), "#%2x%2x%2x%2x", &r, &g, &b, &a);
        return {
            static_cast<unsigned char>(r),
            static_cast<unsigned char>(g),
            static_cast<unsigned char>(b),
            static_cast<unsigned char>(a)
        };
    }
    return {0, 0, 0, 0};
}

inline std::string GetRGBAHexFromImVec4(const ImVec4& vec)
{
    const auto [r, g, b, a] = vec;
    char result[10];
    std::sprintf(result, "#%02X%02X%02X%02X",
                 static_cast<unsigned char>(r * 255),
                 static_cast<unsigned char>(g * 255),
                 static_cast<unsigned char>(b * 255),
                 static_cast<unsigned char>(a * 255));
    return result;
}

/*
inline std::string GetRGBAHexFromColorTuple(const ColorTuple& tup)
{
    const auto [r, g, b, a] = tup;
    char result[10];
    std::sprintf(result, "#%02X%02X%02X%02X", r, g, b, a);
    return result;
}
/**/

inline void SetButtonColors(const ImU32 COLOR_NORMAL, const ImU32 COLOR_HOVER, const ImU32 COLOR_CLICK)
{
    ImGui::PushStyleColor(ImGuiCol_Button, COLOR_NORMAL);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, COLOR_HOVER);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, COLOR_CLICK);
}

inline void ResetButtonColors()
{
    ImGui::PopStyleColor(3);
}

// === === === === === === === === === === === === === === === === === === === === === ===

inline void BenchmarkChangeColorImpl(unsigned char& r, unsigned char& g, unsigned char& b)
{
    if (r > 0) {
        r--;
    }
    else if (g > 0) {
        g--;
    }
    else if (b > 0) {
        b--;
    }
    else {
        r = 255;
        g = 255;
        b = 255;
    }
}

inline void BenchmarkChangeColor(unsigned char& r, unsigned char& g, unsigned char& b, const int modifier)
{
    switch (modifier) {
    default: return BenchmarkChangeColorImpl(r, g, b);
    case 1: return BenchmarkChangeColorImpl(r, b, g);
    case 2: return BenchmarkChangeColorImpl(g, r, b);
    case 3: return BenchmarkChangeColorImpl(g, b, r);
    case 4: return BenchmarkChangeColorImpl(b, r, g);
    case 5: return BenchmarkChangeColorImpl(b, g, r);
    }
}
