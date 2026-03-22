#pragma once

// ReSharper disable once CppUnusedIncludeDirective
#include "../../Dependency/IconsMaterialDesignIcons.h"

#include <optional>
#include <string>

// Used to for-loop the num values. Not ideal, must be updated adding/removing enum value.
constexpr auto N_NTYPES = 4;

// These types of node are available
enum NodeType
{
    NTYPE_RECTANGLE,
    NTYPE_ELLIPSE,
    NTYPE_DIAMOND,
    NTYPE_TEXT,
};

// When parsing TOML, the value of "type=" parameter is converted to this enum
constexpr std::optional<NodeType> GetNodeTypeFromString(const std::string& type_str)
{
    if (type_str.empty() || type_str == "rectangle") return NTYPE_RECTANGLE;
    if (type_str == "ellipse") return NTYPE_ELLIPSE;
    if (type_str == "diamond") return NTYPE_DIAMOND;
    if (type_str == "text") return NTYPE_TEXT;
    return std::nullopt;
}

// Show this error if user provides an unknown type
const std::string NODETYPE_ERROR_MESSAGE =
    "Allowed NodeType values are: 'text', 'rectangle', 'ellipse', 'diamond'";

// Used for icon buttons
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

// Used when adding new node or editing node type via ComboBox (to know how to set "type=" value).
// Also used in tooltips for buttons to add new node.
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
