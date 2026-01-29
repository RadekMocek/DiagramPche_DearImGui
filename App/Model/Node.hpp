#pragma once

#include <string>

#include "NodeType.hpp"
#include "Pivot.hpp"
#include "Point.hpp"
#include "../Config.hpp"

struct Node
{
    // todo
    NodeType type = RECTANGLE;

    // = ID =
    std::string id{};

    // = Value =
    std::string value{};
    bool is_value_explicitly_set = false;

    // = XY =
    Point position{};

    // = Pivot =
    Pivot pivot = TOPLEFT;

    // = Color =
    std::tuple<unsigned char, unsigned char, unsigned char, unsigned char> color = {255, 255, 255, 255};

    // = Size =
    int width{};
    int height{};

    // = Label pos =
    Pivot label_position = CENTER;

    // = Z =
    int z = DRAW_LIST_CHANNEL_DEFAULT_NODE;

    // = Other internal =
    int draw_batch_number = 0;
};
