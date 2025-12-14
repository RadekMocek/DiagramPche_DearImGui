#pragma once

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
};
