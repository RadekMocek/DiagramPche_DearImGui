#pragma once

#include <string>

enum Pivot
{
    TOPLEFT,
    TOP,
    TOPRIGHT,
    RIGHT,
    BOTTOMRIGHT,
    BOTTOM,
    BOTTOMLEFT,
    LEFT,
    CENTER
};

inline std::optional<Pivot> GetPivotFromString(const std::string& pivot_str)
{
    if (pivot_str == "top-left") return TOPLEFT;
    if (pivot_str == "top") return TOP;
    if (pivot_str == "top-right") return TOPRIGHT;
    if (pivot_str == "right") return RIGHT;
    if (pivot_str == "bottom-right") return BOTTOMRIGHT;
    if (pivot_str == "bottom") return BOTTOM;
    if (pivot_str == "bottom-left") return BOTTOMLEFT;
    if (pivot_str == "left") return LEFT;
    if (pivot_str == "center") return CENTER;
    return std::nullopt;
}

const std::string PIVOT_ERROR_MESSAGE =
    "Allowed pivot values are: 'top-left', 'top', 'top-right', 'right', 'bottom-right', 'bottom', 'bottom-left', 'left', 'center'";
