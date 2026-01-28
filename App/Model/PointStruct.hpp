#pragma once

#include <string>

#include "PivotEnum.hpp"
#include "PointRefTypeEnum.hpp"

struct PointStruct
{
    // Node + Path
    std::string parent_id{};
    toml::source_region parent_id_source_region{};
    Pivot parent_pivot{};
    int x{};
    int y{};

    // Path only
    PointRefType ref_type = UNKNOWN_POINTREFTYPE;
};
