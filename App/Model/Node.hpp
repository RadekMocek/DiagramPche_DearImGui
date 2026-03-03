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
    Pivot pivot = PIVOT_TOPLEFT;

    // = Color =
    std::tuple<unsigned char, unsigned char, unsigned char, unsigned char> color = {255, 255, 255, 255};

    // = Size =
    int width{};
    int height{};

    // = Label pos =
    Pivot label_position = PIVOT_CENTER;

    // = Z =
    int z = DL_USER_CHANNEL_DEFAULT_NODE;

    // = Other internal =
    int draw_batch_number = 0;

    // = Canvas interaction =
    toml::source_region node_source{};
    std::optional<toml::source_region> color_source = std::nullopt;
};
