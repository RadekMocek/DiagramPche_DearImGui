#pragma once

#include <unordered_map>
#include <vector>

#include "../../Dependency/toml.hpp"
#include "../Model/Node.hpp"
#include "../Model/Path.hpp"

class Parser
{
public:
    std::unordered_map<std::string, Node> m_result_nodes_map;
    std::vector<Path> m_result_paths;

    // Error report
    bool m_is_error;
    toml::source_region m_error_source_region;
    std::string m_error_description;

    // Methods
    void Parse(const std::string& source);

private:
    std::unordered_map<std::string, int> m_variables;

    void ReportError(const toml::source_region& error_source_region, const std::string& error_description);

    void ParseNode(const toml::table* node_t, Node& cn);
    void ParsePath(const toml::table* path_t, Path& cp);

    void SetPivotFromString(const toml::value<std::string>* value_str_ptr, Pivot& to_set);
    void SetIntFromIntOrVariable(const toml::node& value, int& to_set);
    void SetPositionPointFromArray(const toml::node& value, Point& to_set);

    void ParsePathStartOrEnd(const toml::node& value, Point& to_set);
    void ParsePathpointXOrY(const toml::array* pathpoint_arr_ptr, bool is_x, Pathpoint& cpp);

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
