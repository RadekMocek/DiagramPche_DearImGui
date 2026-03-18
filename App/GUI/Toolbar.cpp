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
    ImGui::BeginChild("Toolbar1Parent", toolbar1_size, 0);
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
    ImGui::PushItemWidth(100); // Make it thinner
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
    // If some node is selected, the toolbar shows its info and allows to change some things (e.g. color via color picker).
    // If no node is selected, the toolbar is disabled, but still shows info if some node is hovered.
    static toml::source_position node_source_end;
    static ColorTuple node_color;
    static std::optional<toml::source_region> node_color_source;
    static int node_type_selected_idx;
    static std::optional<toml::source_region> node_type_source;
    static std::string node_key_label_value;

    // These are the placeholder values to show in toolbar when no node is selected nor hovered
    node_source_end = {0, 0};
    node_color = {240, 240, 240, 255};
    node_color_source = std::nullopt;
    node_type_selected_idx = 0;
    node_type_source = std::nullopt;
    node_key_label_value = "(No node hovered)";

    if (m_is_canvas_node_selected && m_parser.m_result_nodes.contains(m_selected_canvas_node_key)) {
        // Node is selected, get info from it
        const auto& node = m_parser.m_result_nodes[m_selected_canvas_node_key];
        node_source_end = node.node_source.end;
        node_color = node.color;
        node_color_source = node.color_source;
        node_type_selected_idx = node.type;
        node_type_source = node.type_source;
        node_key_label_value = node.id;
    }
    else if (m_selected_or_hovered_canvas_node_key.has_value()
        && m_parser.m_result_nodes.contains(m_selected_or_hovered_canvas_node_key.value())) {
        // Node is not selected, but is at least hovered, get info from it
        // No need to set the "source" values, toolbar is disabled
        const auto& node = m_parser.m_result_nodes[m_selected_or_hovered_canvas_node_key.value()];
        node_color = node.color;
        node_type_selected_idx = node.type;
        node_key_label_value = node.id;
    }

    // Place the toolbar in the UI
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(SEPARATOR_WIDTH, 0));
    ImGui::SameLine();
    const ImVec2 toolbar2_size(ImGui::GetContentRegionAvail().x, TOOLBAR_HEIGHT);
    ImGui::BeginDisabled(!m_is_canvas_node_selected);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, TOOLBAR_PADDING);
    ImGui::BeginChild("Toolbar2Parent", toolbar2_size, 0);
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
    color = GetImVec4FromColorTuple(node_color);
    if (ImGui::ColorEdit4("Node color", reinterpret_cast<float*>(&color),
                          ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel)) {
        if (node_color_source.has_value()) {
            if (color != color_prev) {
                ReplaceInMSource(node_color_source.value(), std::format("\"{}\"", GetRGBAHexFromImVec4(color)));
                color_prev = color;
            }
        }
        else {
            InsertNodeParameterInMSource(node_source_end, std::format("\ncolor = \"{}\"", GetRGBAHexFromImVec4(color)));
        }
        OnMSourceChanged();
    }

    // .: Type select :.
    // .:=============:.
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

    if (GUICombo("##ComboNodeShape", node_types, IM_COUNTOF(node_types), node_type_selected_idx,
                 ImGuiComboFlags_WidthFitPreview)) {
        auto type_str = GetStringFromNodeType(static_cast<NodeType>(node_type_selected_idx));
        if (node_type_source.has_value()) {
            // Change type parameter's value in node's definition
            ReplaceInMSource(node_type_source.value(), std::format("\"{}\"", type_str));
        }
        else {
            // Add type parameter to node's definition
            InsertNodeParameterInMSource(node_source_end, std::format("\ntype = \"{}\"", type_str));
        }
        OnMSourceChanged();
    }

    // .: Node ID label :.
    // .:===============:.
    VerticalSeparator();
    std::ranges::replace(node_key_label_value, '\n', ' ');
    ImGui::Text("ID: %s", node_key_label_value.c_str());

    // --- --- --- --- ---
    ImGui::EndDisabled();
    ImGui::EndChild();
}
