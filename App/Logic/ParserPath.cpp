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

                    // Array of 6 items: two trios (x,y); each trio can be one of:
                    // "", "", 23         :: absolute 23
                    // "id", "pivot", 23  :: relative to 'id' (add 23 to its pivot)
                    // "", "start", 23    :: relative to path start
                    // "", "end", 23      :: relative to path end
                    // "", "prev", 23     :: relative to previous pathpoint
                    // Instead of integer (23), string with variable name can be used
                    if (const auto* pathpoint_arr_ptr = pathpoint.as_array(); pathpoint_arr_ptr) {
                        if (pathpoint_arr_ptr->size() == 6
                            && pathpoint_arr_ptr->at(0).is_string()
                            && pathpoint_arr_ptr->at(1).is_string()
                            && (pathpoint_arr_ptr->at(2).is_integer() || pathpoint_arr_ptr->at(2).is_string())
                            && pathpoint_arr_ptr->at(3).is_string()
                            && pathpoint_arr_ptr->at(4).is_string()
                            && (pathpoint_arr_ptr->at(5).is_integer() || pathpoint_arr_ptr->at(5).is_string())
                        ) {
                            cp.path_points.emplace_back();
                            auto& cpp = cp.path_points.back();
                            ParsePathpointXOrY(pathpoint_arr_ptr, true, cpp);
                            ParsePathpointXOrY(pathpoint_arr_ptr, false, cpp);
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

void Parser::ParsePathpointXOrY(const toml::array* pathpoint_arr_ptr, const bool is_x, Pathpoint& cpp)
{
    // (This method expects correct data types and is taylor-made for parsing the specific 6 item array)
    const auto arr_offset = (is_x) ? 0 : 3;

    const std::string id_str = pathpoint_arr_ptr->at(0 + arr_offset).as_string()->value_or("");

    PathpointType type = REFERENCE;
    Pivot pivot = UNKNOWN_PIVOT;
    if (id_str.empty()) {
        type = GetPathpointTypeFromString(pathpoint_arr_ptr->at(1 + arr_offset).as_string()->value_or(""));
        if (type == UNKNOWN_PATHPOINTTYPE) {
            ReportError(pathpoint_arr_ptr->at(1 + arr_offset).source(), PATHPOINTTYPE_ERROR_MESSAGE);
        }
    }
    else {
        if (!m_result_nodes_map.contains(id_str)) {
            ReportError(pathpoint_arr_ptr->at(0 + arr_offset).source(),
                        std::format("Pathpoint's x is referencing non existant id: '{}'", id_str));
        }
        SetPivotFromString(pathpoint_arr_ptr->at(1 + arr_offset).as_string(), pivot);
    }

    int coor{};
    SetIntFromIntOrVariable(pathpoint_arr_ptr->at(2 + arr_offset), coor);

    if (is_x) {
        cpp.x_type = type;
        cpp.x_parent_id = id_str;
        cpp.x_parent_pivot = pivot;
        cpp.x = coor;
    }
    else {
        cpp.y_type = type;
        cpp.y_parent_id = id_str;
        cpp.y_parent_pivot = pivot;
        cpp.y = coor;
    }
}
