#include <algorithm>
#include <format>

#include "Parser.hpp"
#include "../Config.hpp"
#include "../HelperFunction.hpp"

void Parser::ParseNode(const toml::table* node_table, Node& curr_node)
{
    // Foreach `key` = `value` in current [node]
    for (const auto& [key, value] : *node_table) {
        // Logic is branched according to the key.str() content:
        // == value ==> single string (for now)
        if (const auto key_str = key.str(); key_str == "value") {
            if (const auto* value_str_ptr = value.as_string()) {
                curr_node.value = value_str_ptr->get();
                curr_node.is_value_explicitly_set = true;
            }
            else ReportError(value.source(), "A string must follow after 'value='");
        }
        // == xy ==> array: [X, Y] or [Parent, Pivot, X, Y]; X/Y can be integer or string with variable name
        else if (key_str == "xy") {
            SetPositionPointFromArray(value, curr_node.position);
        }
        // == pivot ==> single string
        else if (key_str == "pivot") {
            if (const auto* value_str_ptr = value.as_string()) {
                SetPivotFromString(value_str_ptr, curr_node.pivot);
            }
            else ReportError(value.source(), "A string must follow after 'pivot='");
        }
        // == color ==> array of four u8s (rgba)
        else if (key_str == "color") {
            if (const auto* value_arr_ptr = value.as_array(); value_arr_ptr &&
                value_arr_ptr->size() == 4 && value_arr_ptr->is_homogeneous(toml::node_type::integer)) {
                curr_node.color_r = value_arr_ptr->at(0).value_or(0);
                curr_node.color_g = value_arr_ptr->at(1).value_or(0);
                curr_node.color_b = value_arr_ptr->at(2).value_or(0);
                curr_node.color_a = value_arr_ptr->at(3).value_or(0);
            }
            else ReportError(value.source(), "An array of four uchars (0–255) must follow after 'color='");
        }
        // == size ==> array of two items [width, height], where each is specified either by integer or a string with variable name
        else if (key_str == "size") {
            if (const auto* value_arr_ptr = value.as_array(); value_arr_ptr && value_arr_ptr->size() == 2) {
                SetIntFromIntOrVariable(value_arr_ptr->at(0), curr_node.width);
                SetIntFromIntOrVariable(value_arr_ptr->at(1), curr_node.height);
            }
            else {
                ReportError(value.source(),
                            "An array of two integers/strings of variable names must follow after 'size='");
            }
        }
        // == label_pos ==> pivot string
        else if (key_str == "label_pos") {
            if (const auto* value_str_ptr = value.as_string()) {
                SetPivotFromString(value_str_ptr, curr_node.label_position);
            }
            else ReportError(value.source(), "A string must follow after 'label_pos='");
        }
        // == z ==> integer
        else if (key_str == "z") {
            curr_node.z = GetZFromInt(value, true);
        }
        // == Unknown key ==> report error
        else ReportError(key.source(), std::format("Unknown key '{}'", key_str));
    }
}
