#include "App.hpp"

void App::Update()
{
    // .: Main menu bar :.
    ModuleMainMenuBar();

    // .: Main "full-viewport" window :.
    ModuleBody();

    // .: Show the big demo window if enabled :.
    if (m_do_show_demo_window) {
        ImGui::PushFont(nullptr, 16.0f);
        ImGui::ShowDemoWindow(&m_do_show_demo_window); // Putting the bool here will add close button to the demo window
        ImGui::PopFont();
    }

    // .: About modal :.
    constexpr auto MODAL_ABOUT_NAME = "About##modal";

    if (m_is_about_popup_queued) {
        m_is_about_popup_queued = false;
        ImGui::OpenPopup(MODAL_ABOUT_NAME);
    }
    // (Always center this window when appearing)
    const ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    if (ImGui::BeginPopupModal(MODAL_ABOUT_NAME, nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("DiagramPche :: Dear ImGui");
        ImGui::TextLinkOpenURL("github.com/RadekMocek/DiagramPche_DearImGui",
                               "https://github.com/RadekMocek/DiagramPche_DearImGui");
        ImGui::Dummy(ImVec2(0.0f, 20.0f));
        if (ImGui::Button("Close", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
        ImGui::EndPopup();
    }
}
