#pragma once

#include <string>

#include "PointRefTypeEnum.hpp"

struct PathPointStruct
{
    PointRefType ref_type = UNKNOWN_POINTREFTYPE;
    std::string parent_id{};
    Pivot parent_pivot{};
    int x{};
    int y{};
};
