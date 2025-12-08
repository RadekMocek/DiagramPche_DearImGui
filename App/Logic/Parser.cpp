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

    const auto nodes = table["node"];

    if (!!nodes && nodes.type() == toml::node_type::array) {
        std::cout << "node: " << nodes << "\n";
    }
}
