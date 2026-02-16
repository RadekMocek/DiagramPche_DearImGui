#include <algorithm>

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

    // Two main columns
    m_parser.Parse(m_source);
    GUITextEditor();
    ImGui::SameLine();
    GUICanvas();

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
