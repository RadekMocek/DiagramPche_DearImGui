#pragma once

#include "../../Dependency/IconsMaterialDesignIcons.h"

#include <optional>
#include <string>

constexpr auto N_NTYPES = 4; // Not ideal, must be updated adding/removing enum value

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
    if (type_str == "ellipse") return NTYPE_ELLIPSE;
    if (type_str == "diamond") return NTYPE_DIAMOND;
    if (type_str == "text") return NTYPE_TEXT;
    return std::nullopt;
}

const std::string NODETYPE_ERROR_MESSAGE =
    "Allowed NodeType values are: 'text', 'rectangle', 'ellipse', 'diamond'";

constexpr const char* GetIconFromNodeType(const NodeType type)
{
    switch (type) {
    case NTYPE_RECTANGLE:
        return ICON_MDI_RECTANGLE_OUTLINE;
    case NTYPE_ELLIPSE:
        return ICON_MDI_ELLIPSE_OUTLINE;
    case NTYPE_DIAMOND:
        return ICON_MDI_RHOMBUS_OUTLINE;
    case NTYPE_TEXT:
        return ICON_MDI_FORMAT_TEXT_VARIANT;
    default:
        return "";
    }
}

constexpr const char* GetStringFromNodeType(const NodeType type)
{
    switch (type) {
    case NTYPE_RECTANGLE:
        return "rectangle";
    case NTYPE_ELLIPSE:
        return "ellipse";
    case NTYPE_DIAMOND:
        return "diamond";
    case NTYPE_TEXT:
        return "text";
    default:
        return "";
    }
}
