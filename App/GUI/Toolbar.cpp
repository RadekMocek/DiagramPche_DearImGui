#include <algorithm>

#include "../App.hpp"
#include "../Helper/Color.hpp"

void App::GUIToolbar()
{
    static Node& toolbar_node = DEFAULT_TOOLBAR_NODE;
    static std::string node_key_label_value;
    static ImVec4 color;
    static ImVec4 color_prev;

    if (m_selected_or_hovered_canvas_node_key.has_value()
        && m_parser.m_result_nodes.contains(m_selected_or_hovered_canvas_node_key.value())) {
        // Case: some node in canvas is selected/hovered and exists
        toolbar_node = m_parser.m_result_nodes[m_selected_or_hovered_canvas_node_key.value()];
        // Update `node_key_label_value` accordingly
        constexpr auto NODE_KEY_LENGTH = 18;
        node_key_label_value = m_selected_or_hovered_canvas_node_key.value().substr(0, NODE_KEY_LENGTH);
        node_key_label_value.resize(NODE_KEY_LENGTH, ' ');
        std::ranges::replace(node_key_label_value, '\n', ' ');
    }
    else {
        // Case: nothing is selected/hovered
        toolbar_node = DEFAULT_TOOLBAR_NODE;
        // In case when the id does not exist anymore
        m_is_canvas_node_selected = false;
        m_selected_or_hovered_canvas_node_key = std::nullopt;
        // Update `node_key_label_value` accordingly
        constexpr auto NODE_KEY_DEFAULT_STRING = "(No node selected)";
        node_key_label_value = NODE_KEY_DEFAULT_STRING;
    }

    constexpr auto TOOLBAR_HEIGHT = 30.0f;
    const ImVec2 toolbar_size(ImGui::GetContentRegionAvail().x, TOOLBAR_HEIGHT);

    ImGui::BeginDisabled(!m_is_canvas_node_selected);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(6.0f, 2.0f));
    ImGui::BeginChild("CanvasBarParent",
                      toolbar_size,
                      ImGuiChildFlags_Borders,
                      ImGuiWindowFlags_None);
    ImGui::PopStyleVar();

    ImGui::AlignTextToFramePadding();
    ImGui::Text("%s", node_key_label_value.c_str());

    ImGui::SameLine();
    color = GetImVec4FromColorTuple(toolbar_node.color);
    if (ImGui::ColorEdit4("Node color", reinterpret_cast<float*>(&color),
                          ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel)) {
        if (toolbar_node.color_source.has_value()) {
            if (color != color_prev) {
                const auto start_opt = GetMSourceIdxFromSourceRegion(toolbar_node.color_source.value().begin);
                // ReSharper disable once CppTooWideScopeInitStatement
                const auto end_opt = GetMSourceIdxFromSourceRegion(toolbar_node.color_source.value().end);
                if (start_opt.has_value() && end_opt.has_value()) {
                    const auto start = start_opt.value() - 1;
                    const auto end = end_opt.value() - 1;
                    const auto length = end - start;
                    m_source.replace(start, length, std::format("\"{}\"", GetRGBAHexFromImVec4(color)));
                }
                color_prev = color;
            }
        }
        else {
            // ReSharper disable once CppTooWideScopeInitStatement
            const auto node_def_end_idx = GetMSourceIdxFromSourceRegion(toolbar_node.node_source.end);
            if (node_def_end_idx.has_value()) {
                m_source.insert(node_def_end_idx.value(), std::format("\ncolor = \"{}\"", GetRGBAHexFromImVec4(color)));
            }
        }

        if (m_do_use_alt_editor) {
            m_alt_editor.SetText(m_source);
        }
    }

    // --- --- --- --- ---
    ImGui::EndDisabled();
    ImGui::EndChild();
}
