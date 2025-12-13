#include <iostream>

#include "../../Dependency/toml.hpp"

#include "Parser.hpp"

void Parser::parse(const std::string& source)
{
    toml::parse_result table;

    try {
        table = toml::parse(source);
    }
    catch (const toml::parse_error& err) {
        std::cerr << "Parsing failed:\n" << err << "\n";
        return;
    }

    m_result_nodes.clear();

    if (const auto nodes = table["node"]; !!nodes && nodes.type() == toml::node_type::array) {
        // `nodes` is an array of tables labeled as `[[node]]`
        // Conversion chore and table iteration:
        if (toml::array* nodes_array = nodes.as_array()) {
            nodes_array->for_each([this](auto&& node) {
                if (auto* node_t = node.as_table()) {
                    // `node_t` is a pointer to the actual table
                    m_result_nodes.push_back(NodeStruct{
                        (*node_t)["id"].value_or(""),
                        (*node_t)["value"].value_or(""),
                        (*node_t)["x"].value_or(0),
                        (*node_t)["y"].value_or(0)
                    });
                }
            });
        }
    }

}
