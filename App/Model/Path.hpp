#pragma once

#include <vector>

#include "Pathpoint.hpp"
#include "Point.hpp"

struct Path
{
    Point start{};
    Point end{};

    std::vector<Pathpoint> path_points;

    /*
     path style:
     * dotted?
     * thickness
     * color

     path ends (arrows etc.)

     curves?
    */
};
