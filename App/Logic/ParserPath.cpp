#include <algorithm>
#include <format>

#include "Parser.hpp"
#include "../Config.hpp"
#include "../HelperFunction.hpp"

void Parser::ParsePath(const toml::table* path_t, PathStruct& cp)
{
    // Foreach `key` = `value` in current [[path]]
    for (const auto& [key, value] : *path_t) {
        // == start ==
        if (const auto key_str = key.str(); key_str == "start") {
            SetPositionPointFromArray(value, cp.start);
            if (const auto& parent_id = cp.start.parent_id;
                !parent_id.empty() && !m_result_nodes_map.contains(parent_id)) {
                ReportError(cp.start.parent_id_source_region,
                            std::format("Path's {} is referencing non existant id: '{}'", key.str(), parent_id));
            }
        }
    }
}
