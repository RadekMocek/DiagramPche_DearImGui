#pragma once

#include <string>

struct NodePriority
{
    int draw_batch_number = 0;
    std::string id{};

    bool operator<(const NodePriority& other) const
    {
        // Negative so lower `draw_batch_number` has priority in priority queue
        return -draw_batch_number < -other.draw_batch_number;
    }
};
