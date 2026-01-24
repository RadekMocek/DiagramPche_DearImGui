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

    Pivot pivot = TOPLEFT;

    std::string base_id{};
    Pivot base_pivot{};

    // Better error reporting for better diagram developer experience :)
    toml::source_region base_id_source_region{};

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
