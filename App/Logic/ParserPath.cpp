#include <format>

#include "Parser.hpp"
#include "../HelperFunction.hpp"

void Parser::ParsePath(const toml::table* path_t, Path& cp)
{
    // Foreach `key` = `value` in current [[path]]
    for (const auto& [key, value] : *path_t) {
        // == start ==
        if (const auto key_str = key.str(); key_str == "start") {
            ParsePathStartOrEnd(value, cp.start);
        }
        // == end ==
        else if (key_str == "end") {
            ParsePathStartOrEnd(value, cp.end);
        }
        // == points ==
        else if (key_str == "points") {
            if (const auto* pathpoints_array = value.as_array(); pathpoints_array) {
                for (const auto& pathpoint : *pathpoints_array) {
                    constexpr auto err_msg_pathpoint_array =
                        "Pathpoint must be an array of 6 items: [string, string, integer, string, string, integer]";

                    if (const auto* pathpoint_arr_ptr = pathpoint.as_array(); pathpoint_arr_ptr) {
                        if (pathpoint_arr_ptr->size() == 6
                            && pathpoint_arr_ptr->at(0).is_string()
                            && pathpoint_arr_ptr->at(1).is_string()
                            && pathpoint_arr_ptr->at(2).is_integer()
                            && pathpoint_arr_ptr->at(3).is_string()
                            && pathpoint_arr_ptr->at(4).is_string()
                            && pathpoint_arr_ptr->at(5).is_integer()
                        ) {
                            //
                        }
                        else ReportError(pathpoint.source(), err_msg_pathpoint_array);
                    }
                    else ReportError(pathpoint.source(), err_msg_pathpoint_array);
                }
            }
            else ReportError(value.source(), "An array of arrays must follow after 'points='");
        }
        // == Unknown key ==> report error
        else ReportError(key.source(), std::format("Unknown key '{}'", key_str));
    }
}

void Parser::ParsePathStartOrEnd(const toml::node& value, Point& to_set)
{
    SetPositionPointFromArray(value, to_set);
    // Check if the parent id exist, `SetPositionPointFromArray` does not do that because all nodes might not parsed yet when we call it (now we know they are)
    if (const auto& parent_id = to_set.parent_id;
        !parent_id.empty() && !m_result_nodes_map.contains(parent_id)) {
        ReportError(to_set.parent_id_source_region,
                    std::format("Path's start/end is referencing non existant id: '{}'", parent_id));
    }
}
