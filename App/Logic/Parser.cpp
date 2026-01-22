#include <format>
#include <ranges>
#include <set>

#include "Parser.hpp"

#include "../Helper/HelperFunction.hpp"

bool Parser::Parse(const std::string& source)
{
    // Reset warning message
    m_has_warning = false;
    m_warning_description = "";

    // Try to parse the TOML input
    toml::parse_result table;
    try {
        table = toml::parse(source);
    }
    catch (const toml::parse_error& err) {
        m_error_source_region = err.source();
        m_error_description = err.description();
        // By returning here, last valid TOML will be drawn
        return false;
    }

    // This map is used by canvas logic to make draw commands
    // It is `map` and not `vector` simply because it's better for the "node reference" implementation
    m_result_nodes_map.clear();

    // Each node can have its coordinates defined absolutely (x=10 y=10) or relatively (ref="some_id" x=10 y=10).
    // For the relative option, we first need to know coordinates of the parent node with id "some_id".
    // But what if it wasn't parsed yet? Or it does not exist? Or it's relative aswell? Or there is a circular import?

    // For this to work, every node has to have some id. If not defined by the user, implicit ids are given: @Node0, @Node1, etc.
    // Character '@' is therefore reserved for internal purposes and cannot be used in custom ids.
    // First we traverse the TOML and set the coordinates regardless of references;
    // but we remember all the references and we will update dependant nodes later.

    // Pairs dependant→refered (node ids)
    std::unordered_map<std::string, std::string> refs{};

    // Ids of nodes that are not dependant on any other node ("stable node": its position is final)
    std::set<std::string> stable_nodes{};

    // Start traversing the TOML
    // Parse the nodes first
    if (const auto nodes = table["node"]; !!nodes && nodes.type() == toml::node_type::array) {
        // `nodes` is an array of tables labeled as `[[node]]`
        // Conversion chore and table iteration:
        if (toml::array* nodes_array = nodes.as_array()) {
            int node_index = -1; // This is for the implicit id creation if custom id is not provided

            for (auto&& node : *nodes_array) {
                if (auto* node_t = node.as_table()) {
                    // `node_t` is a pointer to the actual [[node]] table
                    node_index++; // @Node0, @Node1, ...

                    // Try to get the id and check whether it contains "@"; or give it an implicit id
                    const auto r_id_node_view = (*node_t)["id"];
                    std::string r_id;
                    if (r_id_node_view) {
                        r_id = r_id_node_view.value<std::string>().value();
                        if (r_id.find('@') != std::string::npos) {
                            m_error_source_region = r_id_node_view.node()->source();
                            m_error_description = "Character '@' is reserved: it can't be used in node ids";
                            return false;
                        }
                    }
                    else {
                        r_id = std::format("@Node{}", node_index);
                    }

                    // Check for id duplicates
                    if (m_result_nodes_map.contains(r_id)) {
                        m_error_source_region = r_id_node_view.node()->source();
                        m_error_description = std::format("Duplicate node id: '{}'", r_id);
                        return false;
                    }

                    // Get other values
                    const std::string r_ref = (*node_t)["ref"].value_or("");
                    const auto r_type = GetNodeType((*node_t)["type"].value_or(""));
                    const auto r_x = (*node_t)["x"].value_or(0);
                    const auto r_y = (*node_t)["y"].value_or(0);
                    const auto r_value = (*node_t)["value"].value_or("");

                    // Add node to the result ccollection
                    m_result_nodes_map.emplace(r_id, NodeStruct{r_type, r_id, r_value, r_x, r_y});

                    // Empty `ref` means stable node; otherwise dependant node
                    if (!r_ref.empty()) {
                        refs.insert({r_id, r_ref});
                    }
                    else {
                        stable_nodes.insert(r_id);
                    }
                }
            }
        }
    }

    // Now we irate over `refs` (pairs dependant→refered – p1→p2 for short):
    // If p2 is stable and p1 is unstable, then add p2's coordinates to p1's coordinates and mark p1 as stable.
    // Don't stop until there is a whole iteration, where we don't do this action ↑
    // (So if we have dependecies (C→B) (B→A), first iter makes B stable, second iter makes C stable, third iter does nothing => break)

    // Have we done such action in this iteration (adding coordinates and marking as stable)?
    bool did_anything_change = !refs.empty();

    while (did_anything_change) {
        did_anything_change = false;

        for (const auto& [key, value] : refs) {
            // Issue a warning if the refered id does not exist
            if (!m_result_nodes_map.contains(value)) {
                m_warning_description = std::format("Node '{}' is referencing non existant id: '{}'", key, value);
                m_has_warning = true;
            }

            if (!stable_nodes.contains(key) // Is p1 unstable and
                && stable_nodes.contains(value) // is p2 stable?
                // These two should always be true, but just in case:
                && m_result_nodes_map.contains(key)
                && m_result_nodes_map.contains(value)
            ) { // Add coordinates and mark as stable
                const auto& refered_node = m_result_nodes_map.at(value);
                auto& dependant_node = m_result_nodes_map.at(key);
                dependant_node.x += refered_node.x;
                dependant_node.y += refered_node.y;
                stable_nodes.insert(key);
                did_anything_change = true; // We did the action in this iteration, so this deserves another iteration
            }
        }
    }

    // At this point, if there are still some unresolved references, that means we have a circular reference
    // Pinpointing the exact loop would need aditional logic so we'll just issue a warning with all unstable node ids
    if (!m_has_warning && stable_nodes.size() < m_result_nodes_map.size()) {
        m_warning_description = "Circular reference somewhere among:";
        for (const auto& key : m_result_nodes_map | std::views::keys) {
            if (!stable_nodes.contains(key)) {
                m_warning_description += std::format(" '{}'", key);
            }
        }
        m_has_warning = true;
    }

    // Parse the paths
    m_result_paths.clear();

    if (const auto paths = table["path"]; !!paths && paths.type() == toml::node_type::array) {
        if (toml::array* paths_array = paths.as_array()) {
            // `paths_array` is an array of tables labeled as `[[path]]`
            for (auto&& path : *paths_array) {
                if (auto* path_t = path.as_table()) {
                    //
                    m_result_paths.emplace_back();
                    auto& current_path_struct = m_result_paths.back();

                    if (const auto points = (*path_t)["point"]; !!points && points.type() == toml::node_type::array) {
                        if (toml::array* points_array = points.as_array()) {
                            // `points_array` is an nested array of tables labeled as `[[path.point]]`
                            current_path_struct.points.reserve(points_array->size());

                            for (auto&& point : *points_array) {
                                if (auto* point_t = point.as_table()) {
                                    current_path_struct.points.emplace_back(
                                        (*point_t)["x"].value_or(0),
                                        (*point_t)["y"].value_or(0)
                                    );
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return true;
}
