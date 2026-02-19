#pragma once

enum PathpointType
{
    REFERENCE,
    PPTYPE_ABSOLUTE,
    START,
    END,
    PREVIOUS
};

constexpr std::optional<PathpointType> GetPathpointTypeFromString(const std::string& type_str)
{
    if (type_str.empty()) return PPTYPE_ABSOLUTE;
    if (type_str == "start") return START;
    if (type_str == "end") return END;
    if (type_str == "prev") return PREVIOUS;
    return std::nullopt;
}

const std::string PATHPOINTTYPE_ERROR_MESSAGE =
    "Allowed PathpointType values are: 'start', 'end', 'prev', '' (empty string for absolute coordinates)";
