#include <algorithm>

#include "imgui_internal.h"

#include "../App.hpp"
#include "../Config.hpp"

void App::GUIBody()
{
    // Full-viewport window setup
    constexpr ImGuiWindowFlags flags
        // Main windows must be always on background so it does not get infront of ther modal windows
        = ImGuiWindowFlags_NoBringToFrontOnFocus
        // = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse
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
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
    const ImVec2 content_region_available = ImGui::GetContentRegionAvail();
    const auto textedit_width = content_region_available.x * m_body_split_ratio;
    // - TextEdit
    //GUITextEditor(textedit_width);
    GUITextEditorAlt(textedit_width);
    // - Separator
    ImGui::SameLine();
    ImGui::InvisibleButton("BodySeparator", ImVec2(4.0f, content_region_available.y - BOTTOM_BAR_HEIGHT));
    if (ImGui::IsItemActive()) {
        m_body_split_ratio = (textedit_width + ImGui::GetIO().MouseDelta.x) / content_region_available.x;
        m_body_split_ratio = ImClamp(m_body_split_ratio, 0.1f, 0.9f);
    }
    if (ImGui::IsItemHovered() || ImGui::IsItemActive()) {
        ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
    }
    ImGui::SameLine();
    // - Canvas
    GUICanvas();
    ImGui::PopStyleVar();

    // "Status bar"
    if (m_parser.m_is_error) {
        ImGui::PushStyleColor(ImGuiCol_Text, COLOR_ERROR);
        std::ranges::replace(m_parser.m_error_description, '\n', ' ');
        ImGui::TextUnformatted(m_parser.m_error_description.c_str());
        // Tooltip if the error is too long
        // Wait, what will come of this? https://github.com/ocornut/imgui/issues/9140
        /*
        if (ImGui::BeginItemTooltip()) {
            ImGui::TextWrapped("%s", m_parser.m_error_description.c_str());
            ImGui::EndTooltip();
        }
        /**/
        ImGui::PopStyleColor();
    }

    ImGui::End();
}
