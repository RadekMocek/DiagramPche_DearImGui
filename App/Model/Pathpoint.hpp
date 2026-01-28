#pragma once

#include "PathpointType.hpp"

struct Pathpoint
{
    PathpointType x_type;
    std::string x_parent_id{};
    Pivot x_parent_pivot{};
    int x{};

    PathpointType y_type;
    std::string y_parent_id{};
    Pivot y_parent_pivot{};
    int y{};
};
