#pragma once

#include <vector>

#include "PointStruct.hpp"

struct PathStruct
{
    std::vector<PathPointStruct> points;

    /*
     path style:
     * dotted?
     * thickness
     * color

     path ends (arrows etc.)

     curves?
    */
};
