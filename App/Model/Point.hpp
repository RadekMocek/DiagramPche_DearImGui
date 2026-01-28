#pragma once

#include <string>

#include "Pivot.hpp"

struct Point
{
    std::string parent_id{};
    toml::source_region parent_id_source_region{};
    Pivot parent_pivot{};
    int x{};
    int y{};
};
