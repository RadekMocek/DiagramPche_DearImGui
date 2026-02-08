#pragma once

#include <string>

#include "Pivot.hpp"
#include "Point.hpp"
#include "../Helper/DrawLayer.hpp"

struct Node
{
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
    int z = DL_USER_CHANNEL_DEFAULT_NODE;

    // = Other internal =
    int draw_batch_number = 0;

    // For priority queue
    bool operator<(const Node& other) const
    {
        // Negative so lower `draw_batch_number` has priority in priority queue
        return -draw_batch_number < -other.draw_batch_number;
    }
};
