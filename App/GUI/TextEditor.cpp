#include "imgui_internal.h"
#include "misc/cpp/imgui_stdlib.h"

#include "../App.hpp"
#include "../Config.hpp"

void App::GUITextEditor(const float textedit_width, const float height)
{
    // Same ID for textedit and drawing error highlights
    constexpr auto TEXTEDIT_ID = "##Source";

    // Text editor must be put in a child window for resizing (moving the divider between textedit and canvas) to work (we can use a special flag)
    const auto textedit_top_left = ImGui::GetCursorScreenPos();
    const ImVec2 textedit_size(textedit_width, height);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    constexpr auto flags = ImGuiChildFlags_Borders;
    ImGui::BeginChild("SourceParent", textedit_size, flags);
    ImGui::PopStyleVar();

    if (ImGui::InputTextMultiline(TEXTEDIT_ID,
                                  &m_source,
                                  ImGui::GetContentRegionAvail(),
                                  ImGuiInputTextFlags_AllowTabInput)) {
        m_is_source_dirty = true;
    }

    const auto textedit_real_id = ImGui::GetID(TEXTEDIT_ID);

    // == ERROR HIGHLIGHT ==
    if (m_parser.m_is_error) {
        // Inconsolata is monospace so should be ok
        const auto char_width_x = m_font_inconsolata_medium->
                                  CalcTextSizeA(static_cast<float>(m_source_font_size), FLT_MAX, -1.0f, "A").x;
        const auto text_line_height = ImGui::GetTextLineHeight();

        // Using the same id to get scroll value of the InputTextMultiline and to not draw outside the text edit
        ImGui::BeginChild(TEXTEDIT_ID);

        const auto textedit_scroll_offset_y = ImGui::GetScrollY();
        ImDrawList* draw_list = ImGui::GetWindowDrawList();

        // EH == error highlight
        const auto EH_region = m_parser.m_error_source_region;
        const auto error_y = static_cast<float>(EH_region.begin.line);
        const auto error_x_start = static_cast<float>(EH_region.begin.column - 1);
        const auto error_x_length = static_cast<float>(EH_region.end.column) - error_x_start;

        const auto& textedit_padding = ImGui::GetStyle().FramePadding;
        auto EH_x_start = char_width_x * error_x_start + textedit_padding.x;
        const auto EH_y_start = (error_y - 1) * text_line_height - textedit_scroll_offset_y + textedit_padding.y;

        // --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
        // [!] using `umgui_internal.h` to make EH work with TextInput Horizontal scroll
        if (const auto* input_text_state = ImGui::GetInputTextState(textedit_real_id); input_text_state != nullptr) {
            EH_x_start -= input_text_state->Scroll.x;
        }
        // --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

        const auto EH_top_left = textedit_top_left + ImVec2(EH_x_start, EH_y_start);
        const auto EH_bottom_right = EH_top_left + ImVec2(char_width_x * error_x_length, text_line_height);

        draw_list->AddRectFilled(EH_top_left, EH_bottom_right, COLOR_ERROR_HIGHLIGHT);
        ImGui::EndChild();
    }

    ImGui::EndChild();
}
