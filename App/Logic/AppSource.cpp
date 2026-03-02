#include "../App.hpp"

std::optional<size_t> App::GetMSourceIdxFromSourceRegion(const toml::source_position& position)
{
    size_t result_idx = 0;
    auto line_n = 0;

    while (line_n < position.line - 1) {
        result_idx = m_source.find('\n', result_idx);
        if (result_idx == std::string::npos) return std::nullopt;
        result_idx++;
        line_n++;
    }

    return result_idx + position.column;
}
