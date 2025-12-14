#include "../App.hpp"

void App::ModuleMain()
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

    if (!m_is_parsing_ok) {
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(211, 1, 2, 255));
        ImGui::Text(m_parser.m_error_description.data());
        ImGui::PopStyleColor();
    }

    ImGui::End();
}
