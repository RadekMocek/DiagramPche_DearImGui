#pragma once

#include <vector>

struct PathPointStruct
{
    int x;
    int y;
};

struct PathStruct
{
    std::vector<PathPointStruct> points;
};
