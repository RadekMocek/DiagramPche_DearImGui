#pragma once

#include <vector>

struct PathPointStruct
{
    int x;
    int y;

    /*
    ref
    ref_x
    ref_y
    x
    y

    id?
    text between two last points?

    can do eg:
    ref="@prev"
    ref_y="some_node_od@left"
    */
};

struct PathStruct
{
    std::vector<PathPointStruct> points;

    /*
     path style:
     * dotted?
     * thickness
     * color

     path ends (arrows etc)

     curves?
    */
};
