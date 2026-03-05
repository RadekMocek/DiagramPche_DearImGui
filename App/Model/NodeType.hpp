#pragma once

#include <optional>
#include <string>

enum NodeType
{
    NTYPE_RECTANGLE,
    NTYPE_ELLIPSE,
    NTYPE_DIAMOND,
    NTYPE_TEXT,
};

constexpr std::optional<NodeType> GetNodeTypeFromString(const std::string& type_str)
{
    if (type_str.empty() || type_str == "rectangle") return NTYPE_RECTANGLE;
    if (type_str == "text") return NTYPE_TEXT;
    if (type_str == "ellipse") return NTYPE_ELLIPSE;
    if (type_str == "diamond") return NTYPE_DIAMOND;
    return std::nullopt;
}

const std::string NODETYPE_ERROR_MESSAGE =
    "Allowed NodeType values are: 'text', 'rectangle', 'ellipse', 'diamond'";
