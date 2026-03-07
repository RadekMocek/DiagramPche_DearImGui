#pragma once

#include <string>

#include "NodeType.hpp"
#include "Pivot.hpp"
#include "Point.hpp"
#include "../Helper/DrawLayer.hpp"

using ColorTuple = std::tuple<unsigned char, unsigned char, unsigned char, unsigned char>;

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
    ColorTuple color = {255, 255, 255, 255};
    ColorTuple color_border = {0, 0, 0, 255};

    // = Size =
    int width{};
    int height{};

    // = Label pos =
    Pivot label_position = PIVOT_CENTER;
    int label_shift_x{};
    int label_shift_y{};

    // = Z =
    int z = DL_USER_CHANNEL_DEFAULT_NODE;

    // = Type =
    NodeType type = NTYPE_RECTANGLE;

    // = Other internal =
    int draw_batch_number = 0;

    // = Canvas interaction =
    toml::source_region node_source{};
    std::optional<toml::source_region> color_source = std::nullopt;
    std::optional<toml::source_region> type_source = std::nullopt;
};
