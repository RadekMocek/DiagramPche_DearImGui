#include "Parser.hpp"
#include "../Helper/HelperFunction.hpp"

bool Parser::parse(const std::string& source)
{
    toml::parse_result table;

    try {
        table = toml::parse(source);
    }
    catch (const toml::parse_error& err) {
        m_error_source_region = err.source();
        m_error_description = err.description();
        return false;
    }

    m_result_nodes.clear();

    if (const auto nodes = table["node"]; !!nodes && nodes.type() == toml::node_type::array) {
        // `nodes` is an array of tables labeled as `[[node]]`
        // Conversion chore and table iteration:
        if (toml::array* nodes_array = nodes.as_array()) {
            nodes_array->for_each([this](auto&& node) {
                if (auto* node_t = node.as_table()) {
                    // `node_t` is a pointer to the actual table
                    m_result_nodes.emplace_back(
                        get_node_type((*node_t)["type"].value_or("")),
                        (*node_t)["id"].value_or(""),
                        (*node_t)["value"].value_or(""),
                        (*node_t)["x"].value_or(0),
                        (*node_t)["y"].value_or(0)
                    );
                }
            });
        }
    }

    m_result_paths.clear();

    if (const auto paths = table["path"]; !!paths && paths.type() == toml::node_type::array) {
        if (toml::array* paths_array = paths.as_array()) {
            paths_array->for_each([this](auto&& path) {
                if (auto* path_t = path.as_table()) {
                    //
                    m_result_paths.emplace_back();
                    auto& current_path_struct = m_result_paths.back();

                    if (const auto points = (*path_t)["point"]; !!points && points.type() == toml::node_type::array) {
                        if (toml::array* points_array = points.as_array()) {
                            current_path_struct.points.reserve(points_array->size());

                            points_array->for_each([&current_path_struct](auto&& point) {
                                if (auto* point_t = point.as_table()) {
                                    current_path_struct.points.emplace_back(
                                        (*point_t)["x"].value_or(0),
                                        (*point_t)["y"].value_or(0)
                                    );
                                }
                            });
                        }
                    }
                }
            });
        }
    }

    return true;
}
