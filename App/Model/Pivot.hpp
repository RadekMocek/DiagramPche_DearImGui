#pragma once

#include <optional>
#include <string>

enum Pivot
{
    PIVOT_TOPLEFT,
    PIVOT_TOP,
    PIVOT_TOPRIGHT,
    PIVOT_RIGHT,
    PIVOT_BOTTOMRIGHT,
    PIVOT_BOTTOM,
    PIVOT_BOTTOMLEFT,
    PIVOT_LEFT,
    PIVOT_CENTER
};

inline std::optional<Pivot> GetPivotFromString(const std::string& pivot_str)
{
    if (pivot_str == "top-left") return PIVOT_TOPLEFT;
    if (pivot_str == "top") return PIVOT_TOP;
    if (pivot_str == "top-right") return PIVOT_TOPRIGHT;
    if (pivot_str == "right") return PIVOT_RIGHT;
    if (pivot_str == "bottom-right") return PIVOT_BOTTOMRIGHT;
    if (pivot_str == "bottom") return PIVOT_BOTTOM;
    if (pivot_str == "bottom-left") return PIVOT_BOTTOMLEFT;
    if (pivot_str == "left") return PIVOT_LEFT;
    if (pivot_str == "center") return PIVOT_CENTER;
    return std::nullopt;
}

const std::string PIVOT_ERROR_MESSAGE =
    "Allowed pivot values are: 'top-left', 'top', 'top-right', 'right', 'bottom-right', 'bottom', 'bottom-left', 'left', 'center'";
