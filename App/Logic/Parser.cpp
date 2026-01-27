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
        ReportError(err.source(), std::string(err.description()));
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
                else ReportError(value.source(), "Only integer variables are allowed");
            }
        }
    }

    // == Nodes ==

    // This map is used by canvas logic to make draw commands
    // It is `map` and not `vector` simply because it's better for the "node reference" implementation
    m_result_nodes_map.clear();

    // Each node can have its coordinates defined absolutely (xy=[10,10]) or relatively (xy=["some_id","center",10,10]).
    // For the relative option, `xy`'s first two parameters are parent node's ID and parent node's pivot.

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

    if (const auto node = toml_parsed["node"]; !!node && node.is_table()) {
        if (const auto* node_t = node.as_table()) {
            for (const auto& [node_key, node_value] : *node_t) {
                if (const auto* node_value_t = node_value.as_table()) {
                    // Key exists and is unique (TOML won't parse if duplicate), but it could be empty string (not ideal)
                    const auto node_id = node_key.str();
                    if (node_id.empty()) {
                        ReportError(node_key.source(), "Node id cannot be empty");
                    }
                    else if (node_id == "@") {
                        ReportError(node_key.source(), "Id '@' is reserved for special purposes");
                    }

                    // Currently processed Node == "cn"
                    NodeStruct cn;
                    cn.id = node_id;

                    // Parse `node_value_t` data and set `cn` members; or set error message
                    ParseNode(node_value_t, cn);

                    // Check if the node is not referencing itself
                    if (cn.id == cn.parent_id) {
                        ReportError(cn.parent_id_source_region,
                                    std::format("Node with id '{}' is referencing itself", cn.id));
                    }

                    //
                    if (!cn.is_value_explicitly_set) {
                        cn.value = cn.id;
                    }

                    // Empty parent means stable node; otherwise dependant node
                    if (!cn.parent_id.empty()) {
                        refs.insert({cn.id, cn.parent_id});
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
            if (!m_is_error && !m_result_nodes_map.contains(value)) {
                ReportError(m_result_nodes_map[key].parent_id_source_region,
                            std::format("Node '{}' is referencing non existant id: '{}'", key, value));
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
    if (!m_is_error && stable_nodes.size() < m_result_nodes_map.size()) {
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
            for (const auto& path : *paths_array) {
                if (const auto* path_t = path.as_table()) {
                    ParsePath(path_t);
                }
            }
        }
    }
}

void Parser::ReportError(const toml::source_region& error_source_region, const std::string& error_description)
{
    if (!m_is_error) {
        m_error_source_region = error_source_region;
        m_error_description = error_description;
        m_is_error = true;
    }
}
