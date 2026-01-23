#pragma once

#include <string>

enum NodeType
{
    REF,
    TEXT,
    RECTANGLE
};

struct NodeStruct
{
    NodeType type = NodeType::RECTANGLE;

    std::string id{};
    std::string value{};
    int x{};
    int y{};

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
