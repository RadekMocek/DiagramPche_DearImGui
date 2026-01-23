#pragma once

#include <string>

#include "NodeTypeEnum.hpp"
#include "PivotEnum.hpp"

struct NodeStruct
{
    int draw_batch_number = 0;

    NodeType type = RECTANGLE;

    std::string id{};
    std::string value{};
    int x{};
    int y{};

    Pivot pivot = CENTER;

    std::string base_id{};
    Pivot base_pivot{};

    /*
    text color
    text size
    text bold
    text italic
    rectangle size
    rectangle round corners
    text position within the rectangle
    rectangle color
    rectangle edge color
    rectangle edge style (dotted?)

    // ...
    Rich text? text=["abcd","red12b","defghi","green13i"]; nebo [[style]]

    // ...
    other shapes than rectangle
    custom image
    */
};
