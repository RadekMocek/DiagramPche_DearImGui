#pragma once

#include <vector>

#include "Pathpoint.hpp"
#include "Point.hpp"

struct Path
{
    Point start{};
    std::vector<Point> ends{};
    std::vector<Pathpoint> path_points;
};
