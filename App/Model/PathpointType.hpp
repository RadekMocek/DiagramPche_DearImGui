#pragma once

enum PathpointType
{
    PPTYPE_REFERENCE,
    PPTYPE_ABSOLUTE,
    PPTYPE_START,
    PPTYPE_END,
    PPTYPE_PREVIOUS
};

constexpr std::optional<PathpointType> GetPathpointTypeFromString(const std::string& type_str)
{
    if (type_str.empty()) return PPTYPE_ABSOLUTE;
    if (type_str == "start") return PPTYPE_START;
    if (type_str == "end") return PPTYPE_END;
    if (type_str == "prev") return PPTYPE_PREVIOUS;
    return std::nullopt;
}

const std::string PATHPOINTTYPE_ERROR_MESSAGE =
    "Allowed PathpointType values are: 'start', 'end', 'prev', '' (empty string for absolute coordinates)";
