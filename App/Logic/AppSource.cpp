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

void App::ReplaceInMSource(const toml::source_region& source, const std::string& new_str)
{
    const auto start_opt = GetMSourceIdxFromSourceRegion(source.begin);
    // ReSharper disable once CppTooWideScopeInitStatement
    const auto end_opt = GetMSourceIdxFromSourceRegion(source.end);
    if (start_opt.has_value() && end_opt.has_value()) {
        const auto start = start_opt.value() - 1;
        const auto end = end_opt.value() - 1;
        const auto length = end - start;
        m_source.replace(start, length, new_str);
    }
}

// This method is called when user changes node color or type via toolbar and the node TOML definition does not contain
// the line with "color=" or "type=" (so default is used). In this situation, there is nothing to replace, but we need to
// insert a new line into the source (that explicitly sets the color/type). The `end` parameter tells the function where
// the last character of the line "[node.some_id]" is for the particular node. The parameter `new_str` is then inserted
// to that position. It is expected that `new_str` begins with newline character, so the TOML is still valid after insert.
void App::InsertNodeParameterInMSource(const toml::source_position& end, const std::string& new_str)
{
    // ReSharper disable once CppTooWideScopeInitStatement
    const auto node_def_end_idx = GetMSourceIdxFromSourceRegion(end);
    if (node_def_end_idx.has_value()) {
        m_source.insert(node_def_end_idx.value(), new_str);
    }
}
