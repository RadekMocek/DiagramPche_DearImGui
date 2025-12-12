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
    ModuleTextEditor();
    ImGui::SameLine();
    ModuleCanvas();

    ImGui::End();
}
