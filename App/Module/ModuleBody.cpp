#include "../App.hpp"

void App::ModuleBody()
{
    // Full-viewport window setup
    constexpr ImGuiWindowFlags flags
        = ImGuiWindowFlags_NoBringToFrontOnFocus
        | ImGuiWindowFlags_NoDecoration
        | ImGuiWindowFlags_NoMove
        | ImGuiWindowFlags_NoSavedSettings
        | ImGuiWindowFlags_NoTitleBar;

    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);

    // Full-viewport window
    ImGui::Begin("Main", nullptr, flags);

    // Two main columns
    m_is_parsing_ok = m_parser.parse(m_source);
    ModuleTextEditor();
    ImGui::SameLine();
    ModuleCanvas();

    // "Status bar"
    constexpr auto COLOR_ERROR = IM_COL32(211, 1, 2, 255);
    constexpr auto COLOR_WARNING = IM_COL32(176, 66, 19, 255);
    if (!m_is_parsing_ok) {
        ImGui::PushStyleColor(ImGuiCol_Text, COLOR_ERROR);
        ImGui::Text(m_parser.m_error_description.c_str());
        ImGui::PopStyleColor();
    }
    else if (m_parser.m_has_warning) {
        ImGui::PushStyleColor(ImGuiCol_Text, COLOR_WARNING);
        ImGui::Text(m_parser.m_warning_description.c_str());
        ImGui::PopStyleColor();
    }

    ImGui::End();
}
