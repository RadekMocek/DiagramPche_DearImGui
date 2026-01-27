#include <algorithm>
#include <format>

#include "Parser.hpp"
#include "../Config.hpp"
#include "../HelperFunction.hpp"

void Parser::ParseNode(const toml::table* node_t, NodeStruct& cn)
{
    // Foreach `key` = `value` in current [[node]]
    for (const auto& [key, value] : *node_t) {
        // Logic is branched according to the key.str() content:
        // == value ==> single string (for now)
        if (const auto key_str = key.str(); key_str == "value") {
            if (const auto* value_str_ptr = value.as_string()) {
                cn.value = value_str_ptr->get();
                cn.is_value_explicitly_set = true;
            }
            else ReportError(value.source(), "A string must follow after 'value='");
        }
        // == xy ==> array of two integers: x and y coordinates
        else if (key_str == "xy") {
            if (const auto* value_arr_ptr = value.as_array(); value_arr_ptr) {
                // [X, Y] or [Parent, Pivot, X, Y]
                if (value_arr_ptr->size() == 2) {
                    // X
                    SetIntFromIntOrVariable(value_arr_ptr->at(0), cn.x);
                    // Y
                    SetIntFromIntOrVariable(value_arr_ptr->at(1), cn.y);
                }
                else if (value_arr_ptr->size() == 4) {
                    const auto parent_id_source_region = value_arr_ptr->at(0).source();
                    // Parent
                    if (const auto* value_arr_0_str_ptr = value_arr_ptr->at(0).as_string()) {
                        cn.parent_id = value_arr_0_str_ptr->value_or("");
                        if (cn.parent_id.empty()) {
                            ReportError(parent_id_source_region, "Parent reference can't be empty");
                        }
                        // Better error reporting (self reference/non existing reference) for better diagram developer experience :)
                        cn.parent_id_source_region = parent_id_source_region;
                    }
                    else ReportError(parent_id_source_region, "In [Parent, Pivot, X, Y], 'Parent' must be a string");
                    // Pivot
                    if (const auto* value_arr_1_str_ptr = value_arr_ptr->at(1).as_string()) {
                        SetPivotFromString(value_arr_1_str_ptr, cn.parent_pivot);
                    }
                    else {
                        ReportError(value_arr_ptr->at(1).source(),
                                    "In [Parent, Pivot, X, Y], 'Pivot' must be a string");
                    }
                    // X
                    SetIntFromIntOrVariable(value_arr_ptr->at(2), cn.x);
                    // Y
                    SetIntFromIntOrVariable(value_arr_ptr->at(3), cn.y);
                }
                else ReportError(value.source(), "An array ([X, Y] or [Parent, Pivot, X, Y]) must follow after 'xy='");
            }
            else ReportError(value.source(), "An array ([X, Y] or [Parent, Pivot, X, Y]) must follow after 'xy='");
        }
        // == pivot ==> single string
        else if (key_str == "pivot") {
            if (const auto* value_str_ptr = value.as_string()) {
                SetPivotFromString(value_str_ptr, cn.pivot);
            }
            else ReportError(value.source(), "A string must follow after 'pivot='");
        }
        // == color ==> array of four u8s (rgba)
        else if (key_str == "color") {
            if (const auto* value_arr_ptr = value.as_array(); value_arr_ptr &&
                value_arr_ptr->size() == 4 && value_arr_ptr->is_homogeneous(toml::node_type::integer)) {
                cn.color_r = value_arr_ptr->at(0).value_or(0);
                cn.color_g = value_arr_ptr->at(1).value_or(0);
                cn.color_b = value_arr_ptr->at(2).value_or(0);
                cn.color_a = value_arr_ptr->at(3).value_or(0);
            }
            else ReportError(value.source(), "An array of four uchars (0–255) must follow after 'color='");
        }
        // == size ==> collection of two items [width, height], where each is specified either by integer or a string with variable name
        else if (key_str == "size") {
            if (const auto* value_arr_ptr = value.as_array(); value_arr_ptr && value_arr_ptr->size() == 2) {
                SetIntFromIntOrVariable(value_arr_ptr->at(0), cn.width);
                SetIntFromIntOrVariable(value_arr_ptr->at(1), cn.height);
            }
            else {
                ReportError(value.source(),
                            "An array of two integers/strings of variable names must follow after 'size='");
            }
        }
        // == label_pos ==> pivot string
        else if (key_str == "label_pos") {
            if (const auto* value_str_ptr = value.as_string()) {
                SetPivotFromString(value_str_ptr, cn.label_position);
            }
            else ReportError(value.source(), "A string must follow after 'label_pos='");
        }
        // == z ==> number from -4 to +4
        else if (key_str == "z") {
            if (const auto* value_int_ptr = value.as_integer()) {
                cn.z = std::clamp(value_int_ptr->value_or(0), -Z_DEPTH_ABS_MAX, Z_DEPTH_ABS_MAX);
            }
            else {
                ReportError(value.source(), std::format("An integer between {} and {} must follow after 'z='",
                                                        -Z_DEPTH_ABS_MAX, Z_DEPTH_ABS_MAX));
            }
        }
        else ReportError(key.source(), std::format("Unknown key '{}'", key_str));
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
