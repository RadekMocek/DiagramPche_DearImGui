#pragma once

#include "PathpointType.hpp"

struct Pathpoint
{
    PathpointType x_type;
    std::string x_parent_id{};
    toml::source_region x_parent_id_source_region{};
    Pivot x_parent_pivot{};
    int x{};

    PathpointType y_type;
    std::string y_parent_id{};
    toml::source_region y_parent_id_source_region{};
    Pivot y_parent_pivot{};
    int y{};
};
