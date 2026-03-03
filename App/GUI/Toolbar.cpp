#include <algorithm>

#include "imgui_internal.h"

#include "../App.hpp"
#include "../Config.hpp"
#include "../Helper/Color.hpp"

void App::GUIToolbar(const float textedit_width)
{
    constexpr auto TOOLBAR_HEIGHT = 30.0f;
    constexpr ImVec2 TOOLBAR_PADDING = {6.0f, 2.0f};
    constexpr bool DO_SHOW_BORDERS = true;

    // --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
    // [!] using `umgui_internal.h` for vertical separator, which is still in development (?)
    const auto VerticalSeparator = [] {
        ImGui::SameLine();
        ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical, 1.0f);
        ImGui::SameLine();
    };
    // --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

    // == PART 1 :: TEXTEDIT TOOLBAR ==
    const ImVec2 toolbar1_size(textedit_width, TOOLBAR_HEIGHT);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, TOOLBAR_PADDING);
    // ReSharper disable once CppDFAUnreachableCode
    ImGui::BeginChild("Toolbar1Parent", toolbar1_size, (DO_SHOW_BORDERS) ? ImGuiChildFlags_Borders : 0);
    ImGui::PopStyleVar();

    // --- --- --- ---
    ImGui::EndChild();

    // == PART 2 :: CANVAS TOOLBAR ==

    static Node& toolbar_node = DEFAULT_TOOLBAR_NODE;
    static std::string node_key_label_value;
    static ImVec4 color;
    static ImVec4 color_prev;

    // If some node is selected, the toolbar shows its info and allows to change some things (e.g. color via color picker).
    // If no node is selected, the toolbar is disabled, but still shows info if some node is hovered.

    if (m_selected_or_hovered_canvas_node_key.has_value()
        && m_parser.m_result_nodes.contains(m_selected_or_hovered_canvas_node_key.value())) {
        // Case: some node in canvas is selected/hovered and exists
        toolbar_node = m_parser.m_result_nodes[m_selected_or_hovered_canvas_node_key.value()];
        // Update `node_key_label_value` accordingly
        constexpr auto NODE_KEY_LENGTH = 17;
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
        constexpr auto NODE_KEY_DEFAULT_STRING = "(No node hovered)";
        node_key_label_value = NODE_KEY_DEFAULT_STRING;
    }

    // Place the toolbar in the UI
    const ImVec2 toolbar2_size(ImGui::GetContentRegionAvail().x, TOOLBAR_HEIGHT);

    ImGui::BeginDisabled(!m_is_canvas_node_selected);

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(SEPARATOR_WIDTH, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, TOOLBAR_PADDING);
    ImGui::SameLine();
    // ReSharper disable once CppDFAUnreachableCode
    ImGui::BeginChild("Toolbar2Parent", toolbar2_size, (DO_SHOW_BORDERS) ? ImGuiChildFlags_Borders : 0);
    ImGui::PopStyleVar(2);

    // Text vertical align: center; calling this once seems to be enough (I guess it's because we're using `ImGui::SameLine()`?)
    ImGui::AlignTextToFramePadding();
    ImGui::Text("Node ID: %s", node_key_label_value.c_str());

    // .: Color picker :.
    // .:==============:.
    VerticalSeparator();
    ImGui::Text("Color:");
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

    // .: Type select :.
    // .:=============:.
    VerticalSeparator();
    ImGui::Text("Type:");
    ImGui::SameLine();
    static int sel = 0;
    ImGui::Combo("preferred editor", &sel, "Rectangle\0Oval\0Diamond\0\0"); // TODO ImGuiComboFlags_WidthFitPreview

    // --- --- --- --- ---
    ImGui::EndDisabled();
    ImGui::EndChild();
}
