#pragma once

#include <string>
#include <unordered_map>

enum Pivot
{
    UNKNOWN,
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

const std::unordered_map<std::string, Pivot> STRING_TO_PIVOT = {
    {"top-left", TOPLEFT},
    {"top", TOP},
    {"top-right", TOPRIGHT},
    {"right", RIGHT},
    {"bottom-right", BOTTOMRIGHT},
    {"bottom", BOTTOM},
    {"bottom-left", BOTTOMLEFT},
    {"left", LEFT},
    {"center", CENTER},
};

inline Pivot GetPivot(const std::string& type_str)
{
    const auto it = STRING_TO_PIVOT.find(type_str);
    return it != STRING_TO_PIVOT.end() ? it->second : UNKNOWN;
}

const std::string PIVOT_ERROR_MESSAGE =
        "Allowed pivot values are: 'top-left', 'top', 'top-right', 'right', 'bottom-right', 'bottom', 'bottom-left', 'left', 'center'";
