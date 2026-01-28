#include "misc/cpp/imgui_stdlib.h"

#include "../App.hpp"
#include "../Config.hpp"
#include "../HelperFunction.hpp"

void App::ModuleTextEditor()
{
    // Same ID for textedit and drawing error highlights
    constexpr auto TEXTEDIT_ID = "##Source";

    // Text editor must be put in a child window for resizing (moving the divider between textedit and canvas) to work (we can use a special flag)
    const auto textedit_top_left = ImGui::GetCursorScreenPos();
    const auto content_region_available = ImGui::GetContentRegionAvail();
    const ImVec2 textedit_size(content_region_available.x * 0.5f, content_region_available.y - BOTTOM_BAR_HEIGHT);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::BeginChild("SourceParent", textedit_size, ImGuiChildFlags_Borders | ImGuiChildFlags_ResizeX);
    ImGui::PopStyleVar();

    // TODO on altgr press looses focus -_-
    ImGui::InputTextMultiline(TEXTEDIT_ID,
                              &m_source,
                              ImGui::GetContentRegionAvail(),
                              ImGuiInputTextFlags_AllowTabInput);

    // == ERROR HIGHLIGHT ==
    if (m_parser.m_is_error) {
        // Inconsolata is monospace so should be ok
        // this could be moved into AppStart if font size is not changing at runtime (or update member var on every change)
        const auto char_width_x = m_font_inconsolata_medium->CalcTextSizeA(FONT_SIZE_DEFAULT, FLT_MAX, -1.0f, "A").x;
        const auto text_line_height = ImGui::GetTextLineHeight();
        constexpr auto COLOR_ERROR_HIGHLIGHT = IM_COL32(211, 1, 2, 80);

        // Using the same id to get scroll value of the InputTextMultiline and to not draw outside the text edit
        ImGui::BeginChild(TEXTEDIT_ID);
        const auto textedit_scroll_offset = ImGui::GetScrollY();
        ImDrawList* draw_list = ImGui::GetWindowDrawList();

        // EH == error highlight
        const auto EH_region = m_parser.m_error_source_region;
        const auto error_y = static_cast<float>(EH_region.begin.line);
        const auto error_x_start = static_cast<float>(EH_region.begin.column - 1);
        const auto error_x_length = static_cast<float>(EH_region.end.column) - error_x_start;

        const auto EH_x_start = char_width_x * error_x_start + char_width_x / 2;
        const auto EH_y_start = (error_y - 1) * text_line_height - textedit_scroll_offset + text_line_height / 8;

        const auto EH_top_left = ImVec2Sum(textedit_top_left, {EH_x_start, EH_y_start});
        const auto EH_bottom_right = ImVec2Sum(EH_top_left, {char_width_x * error_x_length, text_line_height});

        draw_list->AddRectFilled(EH_top_left, EH_bottom_right, COLOR_ERROR_HIGHLIGHT);
        ImGui::EndChild();
    }

    ImGui::EndChild();
}
