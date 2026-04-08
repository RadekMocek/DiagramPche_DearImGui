#include <algorithm>

#include "imgui_internal.h"

#include "../App.hpp"
#include "../Config.hpp"

void App::GUIBody()
{
    // --- Body config --- --- --- --- ---
    constexpr auto BOTTOM_BAR_HEIGHT = 18;
    // --- --- --- --- --- --- --- --- ---

    // Full-viewport window setup
    constexpr ImGuiWindowFlags flags
        // Main windows must be always on background so it does not get in front of ther modal windows
        = ImGuiWindowFlags_NoBringToFrontOnFocus
        // ImGuiWindowFlags_NoDecoration = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse:
        | ImGuiWindowFlags_NoDecoration
        | ImGuiWindowFlags_NoMove
        | ImGuiWindowFlags_NoSavedSettings
        | ImGuiWindowFlags_NoScrollWithMouse;

    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);

    // Full-viewport window
    ImGui::Begin("Main", nullptr, flags);

    // Do the TOML parse
    m_parser.Parse(m_source);

    // Two main columns with draggable separator between them
    const auto textedit_width = ImGui::GetContentRegionAvail().x * m_body_split_ratio;
    // - Toolbar
    if (m_do_show_toolbar) {
        GUIToolbar(textedit_width); // This changes GetContentRegionAvail.Y
    }
    const auto content_region_available = ImGui::GetContentRegionAvail();
    const auto main_columns_height = content_region_available.y - BOTTOM_BAR_HEIGHT;
    // - TextEdit
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
    ImGui::PushFont(nullptr, static_cast<float>(m_source_font_size));

    if (m_do_skip_textedit /*(We can skip textedit for benchmark purposes)*/) {
        const ImVec2 textedit_size(textedit_width, main_columns_height);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::BeginChild("SourceParent", textedit_size, 0);
        ImGui::PopStyleVar();
        ImGui::EndChild();
    }
    else if (!m_do_use_alt_editor) {
        GUITextEditor(textedit_width, main_columns_height);
    }
    else {
        GUITextEditorAlt(textedit_width, main_columns_height);
    }

    ImGui::PopFont();
    // - Separator
    ImGui::SameLine();
    ImGui::InvisibleButton("BodySeparator", ImVec2(SEPARATOR_WIDTH, main_columns_height));
    if (ImGui::IsItemActive()) {
        m_body_split_ratio = ImClamp((textedit_width + ImGui::GetIO().MouseDelta.x) / content_region_available.x,
                                     0.1f, 0.9f);
    }
    if (ImGui::IsItemHovered() || ImGui::IsItemActive()) {
        ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
    }
    ImGui::SameLine();
    // - Canvas
    GUICanvas(main_columns_height);
    ImGui::PopStyleVar();

    // "Status bar"
    if (m_parser.m_is_error) {
        ImGui::PushStyleColor(ImGuiCol_Text, COLOR_ERROR);
        std::ranges::replace(m_parser.m_error_description, '\n', ' ');
        ImGui::TextUnformatted(m_parser.m_error_description.c_str());
        // Tooltip with error message on mouse hover; useful if the error is too long to fit into window's width
        //*
        if (ImGui::BeginItemTooltip()) {
            const auto style = ImGui::GetStyle();
            const auto wrap_posistion
                = ImGui::GetWindowViewport()->WorkSize.x
                - style.DisplaySafeAreaPadding.x
                - style.WindowPadding.x;
            ImGui::PushTextWrapPos(wrap_posistion);
            ImGui::TextWrapped("%s", m_parser.m_error_description.c_str());
            ImGui::PopTextWrapPos();
            ImGui::EndTooltip();
        }
        //*/
        ImGui::PopStyleColor();
    }

    ImGui::End();
}
