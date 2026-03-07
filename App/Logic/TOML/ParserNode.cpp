#include <format>

#include "Parser.hpp"

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
        // == color ==> array of four u8s (rgba) or RGBA hex string ("#xxxxxxxx")
        else if (key_str == "color") {
            SetColorFromArray(value, curr_node.color);
            curr_node.color_source = value.source();
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
        // == type ==> node type string
        else if (key_str == "type") {
            if (const auto* value_str_ptr = value.as_string()) {
                if (const auto nodetype_opt = GetNodeTypeFromString(value_str_ptr->value_or(""));
                    nodetype_opt.has_value()) {
                    curr_node.type = nodetype_opt.value();
                    curr_node.type_source = value.source();
                }
                else ReportError(value_str_ptr->source(), NODETYPE_ERROR_MESSAGE);
            }
            else ReportError(value.source(), "A string must follow after 'type='");
        }
        // == color_border ==> same as color
        else if (key_str == "color_border") {
            SetColorFromArray(value, curr_node.color_border);
        }
        // == label_shift ==> array: [X, Y]
        else if (key_str == "label_shift") {
            if (const auto* value_arr_ptr = value.as_array(); value_arr_ptr
                && value_arr_ptr->size() == 2
                && value_arr_ptr->at(0).is_integer()
                && value_arr_ptr->at(1).is_integer()) {
                curr_node.label_shift_x = value_arr_ptr->at(0).as_integer()->value_or(0);
                curr_node.label_shift_y = value_arr_ptr->at(1).as_integer()->value_or(0);
            }
            else ReportError(value.source(), "An array of two integers must follow after 'label_shift='");
        }
        // == Unknown key ==> report error
        else ReportError(key.source(), std::format("Unknown key '{}'", key_str));
    }
}
