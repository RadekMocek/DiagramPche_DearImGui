#pragma once

#include <vector>

#include "../Helper/NodeStruct.hpp"

class Parser
{
public:
    std::vector<NodeStruct> m_result_nodes;

    void parse(const std::string& source);
};
