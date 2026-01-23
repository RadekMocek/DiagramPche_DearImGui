#pragma once

#include <unordered_map>
#include <vector>

#include "../../Dependency/toml.hpp"
#include "../Model/NodeStruct.hpp"
#include "../Model/PathStruct.hpp"

class Parser
{
public:
    std::unordered_map<std::string, NodeStruct> m_result_nodes_map;
    std::vector<PathStruct> m_result_paths;

    // Errors and warnings:
    toml::source_region m_error_source_region;
    std::string m_error_description;
    bool m_has_warning;
    std::string m_warning_description;

    // Methods
    bool Parse(const std::string& source);

private:
    bool ParseNode(const toml::table* node_t, NodeStruct& cn);

    //TODO revize
    const NodeType DEFAULT_DRAW_ITEM_TYPE = RECTANGLE;

    const std::unordered_map<std::string, NodeType> STRING_TO_NODE_TYPE = {
        {"ref", REF},
        {"text", TEXT},
        {"rectangle", RECTANGLE}
    };

    NodeType GetNodeType(const std::string& type_str) const
    {
        const auto it = STRING_TO_NODE_TYPE.find(type_str);
        return it != STRING_TO_NODE_TYPE.end() ? it->second : DEFAULT_DRAW_ITEM_TYPE;
    }
};
