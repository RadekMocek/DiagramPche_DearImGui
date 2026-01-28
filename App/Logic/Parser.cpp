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

    // .: Variables :.
    // .:===========:.
    m_variables.clear();

    if (const auto vars = toml_parsed["variables"]; !!vars && vars.is_table()) {
        if (const auto* vars_table = vars.as_table()) {
            for (const auto& [key, value] : *vars_table) {
                if (const auto* value_int_ptr = value.as_integer()) {
                    m_variables.insert_or_assign(std::string(key.str()), value_int_ptr->value_or(0));
                }
                else ReportError(value.source(), "Only integer variables are allowed");
            }
        }
    }

    // .: Nodes :.
    // .:=======:.
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
        if (const auto* node_table = node.as_table()) {
            for (const auto& [node_key, node_value] : *node_table) {
                if (const auto* node_value_table = node_value.as_table()) {
                    // Key exists and is unique (TOML won't parse if duplicate), but it could be empty string (not ideal)
                    const auto node_id = node_key.str();
                    if (node_id.empty()) {
                        ReportError(node_key.source(), "Node id cannot be empty");
                    }

                    // Currently processed Node
                    Node curr_node;
                    curr_node.id = node_id;

                    // Parse `node_value_table` data and set `curr_node` members; or set error message
                    ParseNode(node_value_table, curr_node);

                    // Check if the node is not referencing itself
                    if (curr_node.id == curr_node.position.parent_id) {
                        ReportError(curr_node.position.parent_id_source_region,
                                    std::format("Node with id '{}' is referencing itself", curr_node.id));
                    }

                    // If user doesn't set any text value explicitly, we use node's ID (can be rejected by setting `value=""`)
                    if (!curr_node.is_value_explicitly_set) {
                        curr_node.value = curr_node.id;
                    }

                    // Empty parent means stable node; otherwise dependant node
                    if (!curr_node.position.parent_id.empty()) {
                        refs.insert({curr_node.id, curr_node.position.parent_id});
                    }
                    else {
                        stable_nodes.insert(curr_node.id);
                    }

                    // Add node to the result collection
                    m_result_nodes_map.emplace(curr_node.id, std::move(curr_node));
                    // Duplicate ID check was done in `ParseNode()`
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
                ReportError(m_result_nodes_map[key].position.parent_id_source_region,
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

    // .: Paths :.
    // .:=======:.
    // Parse the paths
    m_result_paths.clear();

    if (const auto paths = toml_parsed["path"]; !!paths && paths.is_array_of_tables()) {
        if (const auto* paths_array = paths.as_array()) {
            // `paths_array` is an array of tables labeled as `[[path]]`
            for (const auto& path : *paths_array) {
                if (const auto* path_table = path.as_table()) {
                    // Currently processed Path
                    m_result_paths.emplace_back();
                    auto& curr_path = m_result_paths.back();
                    ParsePath(path_table, curr_path);
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

void Parser::SetPivotFromString(const toml::value<std::string>* value_str_ptr, Pivot& to_set)
{
    const auto pivot = GetPivotFromString(value_str_ptr->get());
    if (pivot == UNKNOWN_PIVOT) {
        ReportError(value_str_ptr->source(), PIVOT_ERROR_MESSAGE);
    }
    to_set = pivot;
}

void Parser::SetIntFromIntOrVariable(const toml::node& value, int& to_set)
{
    if (const auto* value_int_ptr = value.as_integer()) {
        to_set = value_int_ptr->value_or(0);
    }
    else if (const auto* value_str_ptr = value.as_string()) {
        if (const auto it = m_variables.find(value_str_ptr->get()); it != m_variables.end()) {
            to_set = it->second;
        }
        else ReportError(value.source(), std::format("Variable '{}' does not exist", value_str_ptr->get()));
    }
    else ReportError(value.source(), "Value must be specified as an integer or a string with a variable name");
}

void Parser::SetPositionPointFromArray(const toml::node& value, Point& to_set)
{
    constexpr auto err_msg_expected_array = "An array ([X, Y] or [Parent, Pivot, X, Y]) expected";

    if (const auto* value_arr_ptr = value.as_array(); value_arr_ptr) {
        // [X, Y] or [Parent, Pivot, X, Y]
        if (value_arr_ptr->size() == 2) {
            // X
            SetIntFromIntOrVariable(value_arr_ptr->at(0), to_set.x);
            // Y
            SetIntFromIntOrVariable(value_arr_ptr->at(1), to_set.y);
        }
        else if (value_arr_ptr->size() == 4) {
            const auto parent_id_source_region = value_arr_ptr->at(0).source();
            // Parent
            if (const auto* value_arr_0_str_ptr = value_arr_ptr->at(0).as_string()) {
                to_set.parent_id = value_arr_0_str_ptr->value_or("");
                if (to_set.parent_id.empty()) {
                    ReportError(parent_id_source_region, "Parent reference can't be empty");
                }
                // Better error reporting (self reference/non existing reference) for better diagram developer experience :)
                to_set.parent_id_source_region = parent_id_source_region;
            }
            else ReportError(parent_id_source_region, "In [Parent, Pivot, X, Y], 'Parent' must be a string");
            // Pivot
            if (const auto* value_arr_1_str_ptr = value_arr_ptr->at(1).as_string()) {
                SetPivotFromString(value_arr_1_str_ptr, to_set.parent_pivot);
            }
            else {
                ReportError(value_arr_ptr->at(1).source(),
                            "In [Parent, Pivot, X, Y], 'Pivot' must be a string");
            }
            // X
            SetIntFromIntOrVariable(value_arr_ptr->at(2), to_set.x);
            // Y
            SetIntFromIntOrVariable(value_arr_ptr->at(3), to_set.y);
        }
        else ReportError(value.source(), err_msg_expected_array);
    }
    else ReportError(value.source(), err_msg_expected_array);
}

int Parser::GetZFromInt(const toml::node& value, const bool is_node)
{
    constexpr int min = 0;
    constexpr int max = N_DRAW_LIST_CHANNELS - 1;
    const auto err_msg_range = std::format("An integer between {} and {} must follow after 'z='", min, max);
    const int default_result = (is_node) ? DRAW_LIST_CHANNEL_DEFAULT_NODE : DRAW_LIST_CHANNEL_DEFAULT_PATH;

    if (const auto* value_int_ptr = value.as_integer()) {
        const int result = value_int_ptr->value_or(default_result);

        if (result < min) {
            ReportError(value.source(), err_msg_range);
            return min;
        }

        if (result > max) {
            ReportError(value.source(), err_msg_range);
            return max;
        }

        return result;
    }

    ReportError(value.source(), err_msg_range);
    return default_result;
}
