#pragma once

#include <unordered_map>
#include <vector>

#include "../../Dependency/toml.hpp"
#include "../Helper/NodeStruct.hpp"

class Parser
{
public:
    std::vector<NodeStruct> m_result_nodes;

    toml::source_region m_error_source_region;
    std::string_view m_error_description;

    bool parse(const std::string& source);

private:
    const NodeType DEFAULT_DRAW_ITEM_TYPE = RECTANGLE;

    const std::unordered_map<std::string, NodeType> string_to_node_type = {
        {"ref", REF},
        {"text", TEXT},
        {"rectangle", RECTANGLE}
    };

    NodeType get_node_type(const std::string& type_str) const
    {
        const auto it = string_to_node_type.find(type_str);
        return it != string_to_node_type.end() ? it->second : DEFAULT_DRAW_ITEM_TYPE;
    }
};
