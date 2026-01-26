#include <format>
#include <ranges>
#include <set>

#include "Parser.hpp"

#include "../HelperFunction.hpp"

void Parser::Parse(const std::string& source)
{
    m_is_error = false;

    // Try to parse the TOML input
    toml::parse_result toml_parsed;
    try {
        toml_parsed = toml::parse(source);
    }
    catch (const toml::parse_error& err) {
        m_error_source_region = err.source();
        m_error_description = err.description();
        m_is_error = true;
        // By returning here, last valid TOML will be drawn (result collections weren't cleared yet)
        return;
    }

    // == Variables ==
    m_variables.clear();

    if (const auto vars = toml_parsed["variables"]; !!vars && vars.is_table()) {
        if (const auto* vars_t = vars.as_table()) {
            for (const auto& [key, value] : *vars_t) {
                if (const auto* value_int_ptr = value.as_integer()) {
                    m_variables.insert_or_assign(std::string(key.str()), value_int_ptr->value_or(0));
                }
                else if (!m_is_error) {
                    m_error_source_region = value.source();
                    m_error_description = "Only integer variables are allowed";
                    m_is_error = true;
                }
            }
        }
    }

    // == Nodes ==

    // This map is used by canvas logic to make draw commands
    // It is `map` and not `vector` simply because it's better for the "node reference" implementation
    m_result_nodes_map.clear();

    // Each node can have its coordinates defined absolutely (xy=[10,10]) or relatively (base=["some_id","center"] xy=[10,10]).
    // For the relative option, `base` takes two parameters: parent node's ID and parent node's pivot.

    // Dependand node will be drawn relative to parent node's pivot; to know the pivot's location, the parent node must be drawn first!
    // This means we have to tell the canvas which nodes must be drawn earlier and which later – we set their `draw_batch_number`.

    // So we set parent's batch number to 0 and dependant's to 1? But how do we know that the parent node is not also dependant on some other node?
    // What if the parent node wasn't parsed yet? Or it does not exist? Or there is a circular dependency?

    // For this to work, every node has to have some ID. If not defined by the user, implicit IDs are given: @Node0, @Node1, etc.
    // Character '@' is therefore reserved for internal purposes and cannot be used in explicit IDs.
    // First we traverse the TOML and set `draw_batch_number` to 0 regardless of references,
    // but we remember all the references and we will update batch number of dependant nodes later.

    // Pairs dependant→parent
    std::unordered_map<std::string, std::string> refs{};

    // IDs of nodes that are not dependant on any other node ("stable node" == its batch number is final)
    std::set<std::string> stable_nodes{};

    // Start traversing the TOML

    // Parse the nodes
    if (const auto nodes = toml_parsed["node"]; !!nodes && nodes.is_array_of_tables()) {
        if (toml::array* nodes_array = nodes.as_array()) {
            // `nodes_array` is an array of tables labeled as `[[node]]`

            int node_index = -1; // This is for the implicit ID creation if explicit ID is not provided

            for (const auto& node : *nodes_array) {
                if (const auto* node_t = node.as_table()) { // `node_t` is a pointer to the actual [[node]] table
                    // Currently processed Node == "cn"
                    NodeStruct cn;

                    // Parse `node_t` data and set `cn` members; or set error message
                    ParseNode(node_t, cn);

                    // Set implicit ID if explicit ID wasn't set by the user
                    if (cn.id.empty()) {
                        node_index++; // @Node0, @Node1, ...
                        cn.id = std::format("@Node{}", node_index);
                    }

                    // Check if the node is not referencing itself
                    if (cn.id == cn.base_id && !m_is_error) {
                        m_error_source_region = cn.base_id_source_region;
                        m_error_description = std::format("Node with id '{}' is referencing itself", cn.id);
                        m_is_error = true;
                    }

                    // Empty `base` means stable node; otherwise dependant node
                    if (!cn.base_id.empty()) {
                        refs.insert({cn.id, cn.base_id});
                    }
                    else {
                        stable_nodes.insert(cn.id);
                    }

                    // Add node to the result collection
                    m_result_nodes_map.emplace(cn.id, std::move(cn)); // Duplicate ID check was done in `ParseNode()`
                }
            }
        }
    }

    // Now we irate over `refs` (pairs dependant→parent – p1→p2 for short):
    // If p2 is stable and p1 is unstable, we make p1's batch number one greater than p2's batch number and mark p1 as stable.
    // Don't stop until there is a whole iteration, where we don't do this action ↑
    // (So if we have dependecies (C→B) (B→A), first iter makes B stable, second iter makes C stable, third iter does nothing => break)

    // Have we done such action in this iteration (updating the batch number and marking node as stable)?
    bool did_anything_change = !refs.empty();

    while (did_anything_change) {
        did_anything_change = false;

        for (const auto& [key, value] : refs) {
            // Check if the refered ID does exist
            if (!m_result_nodes_map.contains(value) && !m_is_error) {
                m_error_source_region = m_result_nodes_map[key].base_id_source_region;
                m_error_description = std::format("Node '{}' is referencing non existant id: '{}'", key, value);
                m_is_error = true;
            }

            if (!stable_nodes.contains(key) // Is p1 unstable and
                && stable_nodes.contains(value) // is p2 stable?
            ) { // Update the batch number and mark as stable
                const auto& refered_node = m_result_nodes_map.at(value);
                auto& dependant_node = m_result_nodes_map.at(key);
                dependant_node.draw_batch_number = refered_node.draw_batch_number + 1;

                stable_nodes.insert(key);
                did_anything_change = true; // We did the action in this iteration, so there will be another iteration
            }
        }
    }

    // At this point, if there are still some unresolved references, that means we have a circular reference
    // Pinpointing the exact loop would need aditional logic so we'll just fill the error message with all unstable node IDs
    if (stable_nodes.size() < m_result_nodes_map.size() && !m_is_error) {
        m_error_source_region = {};
        m_error_description = "Circular reference somewhere among:";
        for (const auto& key : m_result_nodes_map | std::views::keys) {
            if (!stable_nodes.contains(key)) {
                m_error_description += std::format(" '{}'", key);
            }
        }
        m_is_error = true;
    }

    // == Paths ==

    // Parse the paths
    m_result_paths.clear();

    if (const auto paths = toml_parsed["path"]; !!paths && paths.is_array_of_tables()) {
        if (toml::array* paths_array = paths.as_array()) {
            // `paths_array` is an array of tables labeled as `[[path]]`
            for (auto& path : *paths_array) {
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
}
