#pragma once

#include <string>

//#include "../../Dependency/toml.hpp"
//#include "imgui.h"

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

    unsigned char color_r = 255;
    unsigned char color_g = 255;
    unsigned char color_b = 255;
    unsigned char color_a = 255;

    int width{};
    int height{};

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
