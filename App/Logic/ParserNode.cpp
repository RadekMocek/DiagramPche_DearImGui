#include <format>

#include "Parser.hpp"
#include "../HelperFunction.hpp"

bool Parser::ParseNode(const toml::table* node_t, NodeStruct& cn)
{
    // Foreach `key` = `value` in current [[node]]
    for (const auto& [key, value] : *node_t) {
        // Logic is branched according to the key.str() content:
        // == id ==> single string, that cannot contain '@' (reserved character) and must be unique
        if (const auto key_str = key.str(); key_str == "id") {
            if (auto* value_str_ptr = value.as_string()) {
                const auto value_str = value_str_ptr->get();
                // Check for '@'
                if (value_str.find('@') != std::string::npos) {
                    m_error_source_region = value.source();
                    m_error_description = "Character '@' is reserved and it can't be used in node ids";
                    return false;
                }
                // Check for duplicates
                if (m_result_nodes_map.contains(value_str)) {
                    m_error_source_region = value.source();
                    m_error_description = std::format("Duplicate node id: '{}'", value_str);
                    return false;
                }
                // Ok
                cn.id = value_str_ptr->get();
            }
            else {
                m_error_source_region = value.source();
                m_error_description = "A string must follow after 'id='";
                return false;
            }
        }
        // == value ==> single string (for now)
        else if (key_str == "value") {
            if (auto* value_str_ptr = value.as_string()) {
                cn.value = value_str_ptr->get();
            }
            else {
                m_error_source_region = value.source();
                m_error_description = "A string must follow after 'value='";
                return false;
            }
        }
        // == xy ==> array of two integers: x and y coordinates
        else if (key_str == "xy") {
            if (auto* value_arr_ptr = value.as_array(); value_arr_ptr &&
                value_arr_ptr->size() == 2 && value_arr_ptr->is_homogeneous(toml::node_type::integer)) {
                cn.x = value_arr_ptr->at(0).value_or(0);
                cn.y = value_arr_ptr->at(1).value_or(0);
            }
            else {
                m_error_source_region = value.source();
                m_error_description = "An array of two integers must follow after 'xy='";
                return false;
            }
        }
        // == pivot ==> single string
        else if (key_str == "pivot") {
            if (auto* value_str_ptr = value.as_string()) {
                const auto pivot = GetPivot(value_str_ptr->get());
                if (pivot == UNKNOWN) {
                    m_error_source_region = value.source();
                    m_error_description = PIVOT_ERROR_MESSAGE;
                    return false;
                }
                cn.pivot = pivot;
            }
            else {
                m_error_source_region = value.source();
                m_error_description = "A string must follow after 'pivot='";
                return false;
            }
        }
        // == base ==> array if two strings: base id and base pivot
        else if (key_str == "base") {
            if (auto* value_arr_ptr = value.as_array(); value_arr_ptr &&
                value_arr_ptr->size() == 2 && value_arr_ptr->is_homogeneous(toml::node_type::string)) {
                cn.base_id = value_arr_ptr->at(0).value_or("");
                // Better error reporting for better diagram developer experience :)
                cn.base_id_source_region = value_arr_ptr->at(0).source();

                const auto pivot = GetPivot(value_arr_ptr->at(1).value_or(""));
                if (pivot == UNKNOWN) {
                    m_error_source_region = value_arr_ptr->at(1).source();
                    m_error_description = PIVOT_ERROR_MESSAGE;
                    return false;
                }
                cn.base_pivot = pivot;
            }
            else {
                m_error_source_region = value.source();
                m_error_description = "An array of two strings must follow after 'base='";
                return false;
            }
        }
        else {
            m_error_source_region = key.source();
            m_error_description = std::format("Unknown key '{}'", key_str);
            return false;
        }
    }

    return true;
}
