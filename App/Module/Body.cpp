#include <algorithm>

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
    m_parser.Parse(m_source);
    ModuleTextEditor();
    ImGui::SameLine();
    ModuleCanvas();

    // "Status bar"
    constexpr auto COLOR_ERROR = IM_COL32(211, 1, 2, 255);
    //constexpr auto COLOR_WARNING = IM_COL32(176, 66, 19, 255);
    if (m_parser.m_is_error) {
        ImGui::PushStyleColor(ImGuiCol_Text, COLOR_ERROR);
        std::ranges::replace(m_parser.m_error_description, '\n', ' ');
        ImGui::TextUnformatted(m_parser.m_error_description.c_str());
        ImGui::PopStyleColor();
    }

    ImGui::End();
}
