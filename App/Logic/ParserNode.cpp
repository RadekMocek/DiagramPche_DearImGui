#include <format>

#include "Parser.hpp"
#include "../HelperFunction.hpp"

void Parser::ParseNode(const toml::table* node_t, NodeStruct& cn)
{
    // Foreach `key` = `value` in current [[node]]
    for (const auto& [key, value] : *node_t) {
        // Logic is branched according to the key.str() content:
        // == id ==> single string, that cannot contain '@' (reserved character) and must be unique
        if (const auto key_str = key.str(); key_str == "id") {
            if (auto* value_str_ptr = value.as_string()) {
                const auto value_str = value_str_ptr->get();
                if (!m_is_error) {
                    // Check for '@'
                    if (value_str.find('@') != std::string::npos) {
                        m_error_source_region = value.source();
                        m_error_description = "Character '@' is reserved and it can't be used in node ids";
                        m_is_error = true;
                    }
                    // Check for duplicates
                    if (m_result_nodes_map.contains(value_str)) {
                        m_error_source_region = value.source();
                        m_error_description = std::format("Duplicate node id: '{}'", value_str);
                        m_is_error = true;
                    }
                }
                cn.id = value_str_ptr->get();
            }
            else if (!m_is_error) {
                m_error_source_region = value.source();
                m_error_description = "A string must follow after 'id='";
                m_is_error = true;
            }
        }
        // == value ==> single string (for now)
        else if (key_str == "value") {
            if (auto* value_str_ptr = value.as_string()) {
                cn.value = value_str_ptr->get();
            }
            else if (!m_is_error) {
                m_error_source_region = value.source();
                m_error_description = "A string must follow after 'value='";
                m_is_error = true;
            }
        }
        // == xy ==> array of two integers: x and y coordinates
        else if (key_str == "xy") {
            if (auto* value_arr_ptr = value.as_array(); value_arr_ptr &&
                value_arr_ptr->size() == 2 && value_arr_ptr->is_homogeneous(toml::node_type::integer)) {
                cn.x = value_arr_ptr->at(0).value_or(0);
                cn.y = value_arr_ptr->at(1).value_or(0);
            }
            else if (!m_is_error) {
                m_error_source_region = value.source();
                m_error_description = "An array of two integers must follow after 'xy='";
                m_is_error = true;
            }
        }
        // == pivot ==> single string
        else if (key_str == "pivot") {
            if (auto* value_str_ptr = value.as_string()) {
                SetPivot(value_str_ptr, cn.pivot);
            }
            else if (!m_is_error) {
                m_error_source_region = value.source();
                m_error_description = "A string must follow after 'pivot='";
                m_is_error = true;
            }
        }
        // == base ==> array if two strings: base id and base pivot
        else if (key_str == "base") {
            if (auto* value_arr_ptr = value.as_array(); value_arr_ptr &&
                value_arr_ptr->size() == 2 && value_arr_ptr->is_homogeneous(toml::node_type::string)) {
                cn.base_id = value_arr_ptr->at(0).value_or("");
                // Better error reporting for better diagram developer experience :)
                cn.base_id_source_region = value_arr_ptr->at(0).source();
                SetPivot(value_arr_ptr->at(1).as_string(), cn.base_pivot);
            }
            else if (!m_is_error) {
                m_error_source_region = value.source();
                m_error_description = "An array of two strings must follow after 'base='";
                m_is_error = true;
            }
        }
        // == color ==> array of four u8s (rgba)
        else if (key_str == "color") {
            if (auto* value_arr_ptr = value.as_array(); value_arr_ptr &&
                value_arr_ptr->size() == 4 && value_arr_ptr->is_homogeneous(toml::node_type::integer)) {
                cn.color_r = value_arr_ptr->at(0).value_or(0);
                cn.color_g = value_arr_ptr->at(1).value_or(0);
                cn.color_b = value_arr_ptr->at(2).value_or(0);
                cn.color_a = value_arr_ptr->at(3).value_or(0);
            }
            else if (!m_is_error) {
                m_error_source_region = value.source();
                m_error_description = "An array of four uchars (0–255) must follow after 'color='";
                m_is_error = true;
            }
        }
        // == size ==> collection of two items [width, height], where each is specified either by integer or a string with variable name
        else if (key_str == "size") {
            if (auto* value_arr_ptr = value.as_array(); value_arr_ptr && value_arr_ptr->size() == 2) {
                // Width
                if (auto* width_int_ptr = value_arr_ptr->at(0).as_integer()) {
                    cn.width = width_int_ptr->value_or(0);
                }
                else if (auto* width_str_ptr = value_arr_ptr->at(0).as_string()) {
                    //TODO variable
                }
                else if (!m_is_error) {
                    m_error_source_region = value_arr_ptr->at(0).source();
                    m_error_description = "Width must be specified with integer or string with variable name";
                    m_is_error = true;
                }
                // Height
                if (auto* height_int_ptr = value_arr_ptr->at(1).as_integer()) {
                    cn.height = height_int_ptr->value_or(1);
                }
                else if (auto* height_str_ptr = value_arr_ptr->at(1).as_string()) {
                    //TODO variable
                }
                else if (!m_is_error) {
                    m_error_source_region = value_arr_ptr->at(1).source();
                    m_error_description = "Height must be specified with integer or string with variable name";
                    m_is_error = true;
                }
            }
            else if (!m_is_error) {
                m_error_source_region = value.source();
                m_error_description =
                    "An array of two integers and/or strings of variable names must follow after 'size='";
                m_is_error = true;
            }
        }
        // == label_pos ==> pivot string
        else if (key_str == "label_pos") {
            if (auto* value_str_ptr = value.as_string()) {
                SetPivot(value_str_ptr, cn.label_position);
            }
            else if (!m_is_error) {
                m_error_source_region = value.source();
                m_error_description = "A string must follow after 'label_pos='";
                m_is_error = true;
            }
        }
        else if (!m_is_error) {
            m_error_source_region = key.source();
            m_error_description = std::format("Unknown key '{}'", key_str);
            m_is_error = true;
        }
    }
}

void Parser::SetPivot(const toml::value<std::string>* value_str_ptr, Pivot& to_set)
{
    const auto pivot = GetPivotFromString(value_str_ptr->get());
    if (pivot == UNKNOWN && !m_is_error) {
        m_error_source_region = value_str_ptr->source();
        m_error_description = PIVOT_ERROR_MESSAGE;
        m_is_error = true;
    }
    to_set = pivot;
}
