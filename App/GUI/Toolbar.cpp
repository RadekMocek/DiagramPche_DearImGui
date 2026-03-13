#include <algorithm>

#include "imgui_internal.h"
#include "../../Dependency/IconsMaterialDesignIcons.h"

#include "../App.hpp"
#include "../Config.hpp"
#include "../Helper/Color.hpp"
#include "../Helper/GUICombo.hpp"

void App::GUIToolbar(const float textedit_width)
{
    constexpr auto TOOLBAR_HEIGHT = 30.0f;
    constexpr ImVec2 TOOLBAR_PADDING = {6.0f, 2.0f};
    // For some reason, `ImGui::Dummy` with zero vector still gives some space
    constexpr ImVec2 ADDITIONAL_LEFT_PADDING = {0.0f, 0.0f};
    constexpr bool DO_SHOW_BORDERS = false;

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

    // Text vertical align: center; calling this once in child seems to be enough (I guess it's because we're using `ImGui::SameLine()`?)
    ImGui::AlignTextToFramePadding();
    // Additional padding
    ImGui::Dummy(ADDITIONAL_LEFT_PADDING);
    ImGui::SameLine();

    // .: Text editor font size :.
    // .:=======================:.
    ImGui::Text("Font size:");
    ImGui::SameLine();
    ImGui::PushItemWidth(100);
    WidgetTextEditorFontSizeInputInt();
    ImGui::PopItemWidth();

    // .: Cursor position in text edit info :.
    // .:===================================:.
    if (m_do_use_alt_editor) {
        VerticalSeparator();
        const auto cursor_pos = m_alt_editor.GetCursorPosition();
        ImGui::Text("Cursor pos: %s", std::format("{},{}", cursor_pos.mLine, cursor_pos.mColumn).c_str());
    }

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
        node_key_label_value = m_selected_or_hovered_canvas_node_key.value();
        std::ranges::replace(node_key_label_value, '\n', ' ');
    }
    else {
        // Case: nothing is selected/hovered
        toolbar_node = DEFAULT_TOOLBAR_NODE; //???[1]: this does not work as expected
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

    // Text vertical align: center; calling this once in child seems to be enough (I guess it's because we're using `ImGui::SameLine()`?)
    ImGui::AlignTextToFramePadding();
    // Additional padding
    ImGui::Dummy(ADDITIONAL_LEFT_PADDING);
    ImGui::SameLine();

    // .: Color picker :.
    // .:==============:.
    static ImVec4 color;
    static ImVec4 color_prev;

    ImGui::Text("Node color:");
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
        OnMSourceChanged();
    }

    // .: Type select :.
    // .:=============:.
    static int node_type_selected_idx;
    // This must correspond to `enum NodeType` values
    const char* node_types[] = {
        ICON_MDI_RECTANGLE_OUTLINE" Rectangle",
        ICON_MDI_ELLIPSE_OUTLINE" Ellipse  ",
        ICON_MDI_RHOMBUS_OUTLINE" Diamond  ",
        ICON_MDI_FORMAT_TEXT_VARIANT" Text     "
    };

    VerticalSeparator();
    ImGui::Text("Type:");
    ImGui::SameLine();

    node_type_selected_idx = toolbar_node.type;

    if (GUICombo("##ComboNodeShape", node_types, IM_COUNTOF(node_types), node_type_selected_idx,
                 ImGuiComboFlags_WidthFitPreview)) {
        auto type_str = GetStringFromNodeType(static_cast<NodeType>(node_type_selected_idx));
        if (toolbar_node.type_source.has_value()) {
            // Change type parameter's value in node's definition
            ReplaceInMSource(toolbar_node.type_source.value(), std::format("\"{}\"", type_str));
        }
        else {
            // Add type parameter to node's definition
            InsertNodeParameterInMSource(toolbar_node, std::format("\ntype = \"{}\"", type_str));
        }
        OnMSourceChanged();
    }

    // .: Node ID label :.
    // .:===============:.
    VerticalSeparator();
    ImGui::Text("ID: %s", node_key_label_value.c_str());

    // --- --- --- --- ---
    ImGui::EndDisabled();
    ImGui::EndChild();
}
