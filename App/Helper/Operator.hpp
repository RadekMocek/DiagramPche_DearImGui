#pragma once

#include "imgui.h"

inline bool operator>(const ImVec2& lhs, const ImVec2& rhs) { return lhs.x > rhs.x && lhs.y > rhs.y; }
inline bool operator<(const ImVec2& lhs, const ImVec2& rhs) { return lhs.x < rhs.x && lhs.y < rhs.y; }
inline bool operator>=(const ImVec2& lhs, const ImVec2& rhs) { return lhs.x >= rhs.x && lhs.y >= rhs.y; }
inline bool operator<=(const ImVec2& lhs, const ImVec2& rhs) { return lhs.x <= rhs.x && lhs.y <= rhs.y; }

// GCC ok
// MSVC says: binary '<<': no operator found which takes a right-hand operand of type 'const char [2]' (or there is no acceptable conversion)
/*
inline std::ostream& operator<<(std::ostream& os, const ImVec2& v)
{
    return os << "[" << v.x << ", " << v.y << "]";
}
*/
