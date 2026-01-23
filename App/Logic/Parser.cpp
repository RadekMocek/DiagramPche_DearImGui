#include <format>
#include <ranges>
#include <set>

#include "Parser.hpp"

#include "../HelperFunction.hpp"

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
        //TODO It would be nice if the errors further down also would not clear the whole canvas
        return false;
    }

    // This map is used by canvas logic to make draw commands
    // It is `map` and not `vector` simply because it's better for the "node reference" implementation
    m_result_nodes_map.clear();

    // Each node can have its coordinates defined absolutely (xy=[10,10]) or relatively (base=["some_id","center"] xy=[10,10]).
    // For the relative option, `base` takes two parameters: parent node's id and parent node's pivot.

    // Dependand node will be drawn relative to parent node's pivot; to know the pivot's location, the parent node must be drawn first!
    // This means we have to tell the canvas which nodes must be drawn earlier and which later – we set their `draw_batch_number`.

    // So we set parent's batch number to 0 and dependant's to 1? But how do we know that the parent node is not also dependant on some other node?
    // What if the parent node wasn't parsed yet? Or it does not exist? Or there is a circular dependency?

    // For this to work, every node has to have some id. If not defined by the user, implicit ids are given: @Node0, @Node1, etc.
    // Character '@' is therefore reserved for internal purposes and cannot be used in custom ids.
    // First we traverse the TOML and set `draw_batch_number` to 0 regardless of references,
    // but we remember all the references and we will update this member of dependant nodes later.

    // Pairs dependant→parent
    std::unordered_map<std::string, std::string> refs{};

    // Ids of nodes that are not dependant on any other node ("stable node" == its batch number is final)
    std::set<std::string> stable_nodes{};

    // Start traversing the TOML
    // Parse the nodes first
    if (const auto nodes = table["node"]; !!nodes && nodes.type() == toml::node_type::array) {
        if (toml::array* nodes_array = nodes.as_array()) {
            // `nodes_array` is an array of tables labeled as `[[node]]`

            int node_index = -1; // This is for the implicit id creation if custom id is not provided

            for (auto&& node : *nodes_array) {
                if (const auto* node_t = node.as_table()) { // `node_t` is a pointer to the actual [[node]] table
                    // Currently processed Node == "cn"
                    NodeStruct cn;

                    // Parse `node_t` data and set `cn` members; or set error message and return false
                    if (!ParseNode(node_t, cn)) return false;

                    // Implicit id if not set by the user
                    if (cn.id.empty()) {
                        node_index++; // @Node0, @Node1, ...
                        cn.id = std::format("@Node{}", node_index);
                    }

                    // Add node to the result collection
                    m_result_nodes_map.insert({cn.id, cn}); //TODO

                    // Empty `ref` means stable node; otherwise dependant node
                    if (!cn.base_id.empty()) {
                        refs.insert({cn.id, cn.base_id});
                    }
                    else {
                        stable_nodes.insert(cn.id);
                    }
                }
            }
        }
    }

    // Now we irate over `refs` (pairs dependant→parent – p1→p2 for short):
    // If p2 is stable and p1 is unstable, the make p1's batch number one greater than p2's batch number and mark p1 as stable.
    // Don't stop until there is a whole iteration, where we don't do this action ↑
    // (So if we have dependecies (C→B) (B→A), first iter makes B stable, second iter makes C stable, third iter does nothing => break)

    // Have we done such action in this iteration (updating the batch number and marking node as stable)?
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
            ) { // Update the batch number and mark as stable
                const auto& refered_node = m_result_nodes_map.at(value);
                auto& dependant_node = m_result_nodes_map.at(key);
                dependant_node.draw_batch_number = refered_node.draw_batch_number + 1;

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
