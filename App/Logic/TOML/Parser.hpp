#pragma once

#include <queue>
#include <unordered_map>
#include <vector>

#include "../../../Dependency/toml.hpp"
#include "../../Model/Node.hpp"
#include "../../Model/Path.hpp"
#include "NodePriority.hpp"

using ColorTuple = std::tuple<unsigned char, unsigned char, unsigned char, unsigned char>;

class Parser
{
public:
    // Parsing results
    // [node]
    std::unordered_map<std::string, Node> m_result_nodes;
    std::priority_queue<NodePriority> m_result_nodes_pq;
    // [[path]]
    std::vector<Path> m_result_paths;

    // Error report
    bool m_is_error;
    toml::source_region m_error_source_region;
    std::string m_error_description;

    // Methods
    void Parse(const std::string& source);

private:
    // [variables]
    std::unordered_map<std::string, int> m_variables;

    void UpdatePQ();

    void ReportError(const toml::source_region& error_source_region, const std::string& error_description);

    void ParseNode(const toml::table* node_table, Node& curr_node);
    void ParsePath(const toml::table* path_table, Path& curr_path);

    void SetPivotFromString(const toml::value<std::string>* value_str_ptr, Pivot& to_set);
    void SetIntFromIntOrVariable(const toml::node& value, int& to_set);
    void SetPositionPointFromArray(const toml::node& value, Point& to_set);
    void SetColorFromArrayOrString(const toml::node& value, ColorTuple& to_set);
    int GetZFromInt(const toml::node& value, bool is_node);

    void ParsePathStartOrEnd(const toml::node& value, Point& to_set);
    void ParsePathpointXOrY(const toml::array* pathpoint_arr_ptr, bool is_x, Pathpoint& curr_pathpoint);
};
