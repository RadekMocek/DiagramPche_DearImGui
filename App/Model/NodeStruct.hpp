#pragma once

#include <string>

//#include "../../Dependency/toml.hpp"
//#include "imgui.h"

#include "NodeTypeEnum.hpp"
#include "PivotEnum.hpp"

struct NodeStruct
{
    // todo
    NodeType type = RECTANGLE;

    // = ID =
    std::string id{};

    // = Value =
    std::string value{};
    bool is_value_explicitly_set = false;

    // = XY =
    std::string parent_id{};
    Pivot parent_pivot{};
    int x{};
    int y{};

    // = Pivot =
    Pivot pivot = TOPLEFT;

    // = Color =
    unsigned char color_r = 255;
    unsigned char color_g = 255;
    unsigned char color_b = 255;
    unsigned char color_a = 255;

    // = Size =
    int width{};
    int height{};

    // = Label pos =
    Pivot label_position = CENTER;

    // = Z =
    int z = 1;

    // = Other internal =
    int draw_batch_number = 0;
    // Better error reporting (self reference/non existing reference) for better diagram developer experience :)
    toml::source_region parent_id_source_region{};

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
