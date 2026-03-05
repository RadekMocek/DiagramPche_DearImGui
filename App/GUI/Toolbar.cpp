#include <algorithm>

#include "imgui_internal.h"

#include "../App.hpp"
#include "../Config.hpp"
#include "../Helper/Color.hpp"
#include "../Helper/GUICombo.hpp"

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
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(SEPARATOR_WIDTH, 0));
    ImGui::SameLine();
    const ImVec2 toolbar2_size(ImGui::GetContentRegionAvail().x, TOOLBAR_HEIGHT);
    ImGui::BeginDisabled(!m_is_canvas_node_selected);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, TOOLBAR_PADDING);
    // ReSharper disable once CppDFAUnreachableCode
    ImGui::BeginChild("Toolbar2Parent", toolbar2_size, (DO_SHOW_BORDERS) ? ImGuiChildFlags_Borders : 0);
    ImGui::PopStyleVar(2);

    // Text vertical align: center; calling this once seems to be enough (I guess it's because we're using `ImGui::SameLine()`?)
    ImGui::AlignTextToFramePadding();
    ImGui::Text("Node ID: %s", node_key_label_value.c_str());

    const auto UpdateAltTextEditIfNeeded = [this] {
        if (m_do_use_alt_editor) {
            //const auto cursor_pos = m_alt_editor.GetCursorPosition();
            m_alt_editor.SetText(m_source);
            //m_alt_editor.SetCursorPosition(cursor_pos);

            // This resets scroll even if I do the GetCursorPosition + SetCursorPosition ¯\_(ツ)_/¯
        }
    };

    // .: Color picker :.
    // .:==============:.
    static ImVec4 color;
    static ImVec4 color_prev;

    VerticalSeparator();
    ImGui::Text("Color:");
    ImGui::SameLine();
    color = GetImVec4FromColorTuple(toolbar_node.color);
    if (ImGui::ColorEdit4("Node color", reinterpret_cast<float*>(&color),
                          ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel)) {
        if (toolbar_node.color_source.has_value()) {
            if (color != color_prev) {
                ReplaceInMSource(toolbar_node.color_source.value(), std::format("\"{}\"", GetRGBAHexFromImVec4(color)));
                color_prev = color;
            }
        }
        else {
            InsertNodeParameterInMSource(toolbar_node, std::format("\ncolor = \"{}\"", GetRGBAHexFromImVec4(color)));
        }
        UpdateAltTextEditIfNeeded();
    }

    // .: Type select :.
    // .:=============:.
    static int node_type_selected_idx;
    // This must correspond to `enum NodeType` values
    const char* node_types[] = {"Rectangle", "Ellipse  ", "Diamond  ", "Text     "};

    VerticalSeparator();
    ImGui::Text("Type:");
    ImGui::SameLine();

    node_type_selected_idx = toolbar_node.type;

    if (GUICombo("##ComboNodeShape", node_types, IM_COUNTOF(node_types), node_type_selected_idx,
                 ImGuiComboFlags_WidthFitPreview)) {
        auto type_str = "rectangle";
        if (node_type_selected_idx == 1) type_str = "ellipse";
        else if (node_type_selected_idx == 2) type_str = "diamond";
        else if (node_type_selected_idx == 3) type_str = "text";
        if (toolbar_node.type_source.has_value()) {
            // Change type parameter's value in node's definition
            ReplaceInMSource(toolbar_node.type_source.value(), std::format("\"{}\"", type_str));
        }
        else {
            // Add type parameter to node's definition
            InsertNodeParameterInMSource(toolbar_node, std::format("\ntype = \"{}\"", type_str));
        }
        UpdateAltTextEditIfNeeded();
    }

    // --- --- --- --- ---
    ImGui::EndDisabled();
    ImGui::EndChild();
}
