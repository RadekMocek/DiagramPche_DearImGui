#include <format>

#include "Parser.hpp"

void Parser::ParsePath(const toml::table* path_table, Path& curr_path)
{
    // Foreach `key` = `value` in current [[path]]
    for (const auto& [key, value] : *path_table) {
        // == start ==> position point
        if (const auto key_str = key.str(); key_str == "start") {
            ParsePathStartOrEnd(value, curr_path.start);
        }
        // == end ==> position point
        else if (key_str == "end") {
            curr_path.ends.emplace_back();
            auto& curr_point = curr_path.ends.back();
            ParsePathStartOrEnd(value, curr_point);
        }
        // == ends ==> array of position points
        else if (key_str == "ends") {
            if (const auto* value_arr_ptr = value.as_array()) {
                for (const auto& inner_value : *value_arr_ptr) {
                    curr_path.ends.emplace_back();
                    auto& curr_point = curr_path.ends.back();
                    ParsePathStartOrEnd(inner_value, curr_point);
                }
            }
            else ReportError(value.source(), "An array of end Pathpoints must follow after 'ends='");
        }
        // == points ==> array of 6 item arrays
        else if (key_str == "points") {
            if (const auto* pathpoints_arr = value.as_array()) {
                for (const auto& pathpoint : *pathpoints_arr) {
                    // Array of 6 items: two trios (x,y); each trio can be one of:
                    // "",   "",      23  :: absolute 23
                    // "ID", "pivot", 23  :: relative to 'ID' (add 23 to its pivot)
                    // "",   "start", 23  :: relative to path start
                    // "",   "end",   23  :: relative to path end
                    // "",   "prev",  23  :: relative to previous pathpoint
                    // Instead of integer (23), string with variable name can be used
                    if (const auto* pathpoint_arr_ptr = pathpoint.as_array(); !!pathpoint_arr_ptr
                        && pathpoint_arr_ptr->size() == 6
                        && pathpoint_arr_ptr->at(0).is_string()
                        && pathpoint_arr_ptr->at(1).is_string()
                        && (pathpoint_arr_ptr->at(2).is_integer() || pathpoint_arr_ptr->at(2).is_string())
                        && pathpoint_arr_ptr->at(3).is_string()
                        && pathpoint_arr_ptr->at(4).is_string()
                        && (pathpoint_arr_ptr->at(5).is_integer() || pathpoint_arr_ptr->at(5).is_string())
                    ) {
                        curr_path.pathpoints.emplace_back();
                        auto& curr_pathpoint = curr_path.pathpoints.back();
                        ParsePathpointXOrY(pathpoint_arr_ptr, true, curr_pathpoint);
                        ParsePathpointXOrY(pathpoint_arr_ptr, false, curr_pathpoint);
                    }
                    else {
                        ReportError(pathpoint.source(),
                                    "Pathpoint must be an array of 6 items: [string, string, integer, string, string, integer] "
                                    "(and 'points' expects an ARRAY of pathpoints)");
                    }
                }
            }
            else ReportError(value.source(), "An array of arrays must follow after 'points='");
        }
        // == shift ==> int or var
        else if (key_str == "shift") {
            SetIntFromIntOrVariable(value, curr_path.shift);
        }
        // == color ==> array of four u8s (rgba)
        else if (key_str == "color") {
            SetColorFromArray(value, curr_path.color);
        }
        // == tips ==> 2 char string for now?
        else if (key_str == "tips") {
            if (const auto* value_str_ptr = value.as_string()) {
                if (const std::string value_str = value_str_ptr->value_or(""); value_str == "--") {
                    curr_path.do_start_arrow = false;
                    curr_path.do_end_arrow = false;
                }
                else if (value_str == "<-") {
                    curr_path.do_start_arrow = true;
                    curr_path.do_end_arrow = false;
                }
                else if (value_str == "->") {
                    curr_path.do_start_arrow = false;
                    curr_path.do_end_arrow = true;
                }
                else if (value_str == "<>") {
                    curr_path.do_start_arrow = true;
                    curr_path.do_end_arrow = true;
                }
                else ReportError(value.source(), "Possible values after 'tips=' are '--', '<-', '->', '<>'");
            }
            else ReportError(value.source(), "A string must follow after 'tips='");
        }
        // == Unknown key ==> report error
        else ReportError(key.source(), std::format("Unknown key '{}'", key_str));
    }
}

void Parser::ParsePathStartOrEnd(const toml::node& value, Point& to_set)
{
    SetPositionPointFromArray(value, to_set);
    // Check if the parent id exist, `SetPositionPointFromArray` does not do that because all nodes might not parsed yet when we call it (now they are)
    if (const auto& parent_id = to_set.parent_id;
        !parent_id.empty() && !m_result_nodes_map.contains(parent_id)) {
        ReportError(to_set.parent_id_source_region,
                    std::format("Path's start/end is referencing non existant id: '{}'", parent_id));
    }
}

void Parser::ParsePathpointXOrY(const toml::array* pathpoint_arr_ptr, const bool is_x, Pathpoint& curr_pathpoint)
{
    // (This method expects correct data types and is taylor-made for parsing the specific 6 item array)
    // (This method is called twice on the 6 item array, and according to `is_x` parses either first or second half of it)
    const auto arr_offset = (is_x) ? 0 : 3;

    const std::string id_str = pathpoint_arr_ptr->at(0 + arr_offset).as_string()->value_or("");

    PathpointType type = REFERENCE;
    Pivot pivot = UNKNOWN_PIVOT;
    if (id_str.empty()) { // Empty ID => not a REFERENCE pathpoint type
        type = GetPathpointTypeFromString(pathpoint_arr_ptr->at(1 + arr_offset).as_string()->value_or(""));
        if (type == UNKNOWN_PATHPOINTTYPE) {
            ReportError(pathpoint_arr_ptr->at(1 + arr_offset).source(), PATHPOINTTYPE_ERROR_MESSAGE);
        }
    }
    else { // Some ID => pathpoint type is REFERENCE => check if ID is valid and get parent's pivot
        if (!m_result_nodes_map.contains(id_str)) {
            ReportError(pathpoint_arr_ptr->at(0 + arr_offset).source(),
                        std::format("Pathpoint's x is referencing non existant id: '{}'", id_str));
        }
        SetPivotFromString(pathpoint_arr_ptr->at(1 + arr_offset).as_string(), pivot);
    }

    // Set coordinate (either x or y, depends on the `is_x`), can be number or a variable
    int coor{};
    SetIntFromIntOrVariable(pathpoint_arr_ptr->at(2 + arr_offset), coor);

    if (is_x) {
        curr_pathpoint.x_type = type;
        curr_pathpoint.x_parent_id = id_str;
        curr_pathpoint.x_parent_pivot = pivot;
        curr_pathpoint.x = coor;
    }
    else {
        curr_pathpoint.y_type = type;
        curr_pathpoint.y_parent_id = id_str;
        curr_pathpoint.y_parent_pivot = pivot;
        curr_pathpoint.y = coor;
    }
}
