#pragma once

#include <vector>

#include "PointStruct.hpp"

struct PathStruct
{
    PointStruct start{};

    //
    // todo
    std::vector<PointStruct> points;

    /*
     path style:
     * dotted?
     * thickness
     * color

     path ends (arrows etc.)

     curves?
    */
};
