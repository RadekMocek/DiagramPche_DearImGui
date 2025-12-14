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
    NodeType type;

    std::string id;
    std::string value;
    int x;
    int y;
};
