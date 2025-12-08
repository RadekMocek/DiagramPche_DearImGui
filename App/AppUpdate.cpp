#include "App.hpp"

void App::Update()
{
    // .: Update state :.
    static bool do_show_demo_window = false;
    static bool is_about_popup_queued = false;

    // .: Main menu bar :.
    ImGui::PushFont(nullptr, 16.0f);
    ImGui::BeginMainMenuBar();
    // . File .
    if (ImGui::BeginMenu("File")) {
        if (ImGui::MenuItem("Exit", "Alt+F4")) {
            glfwSetWindowShouldClose(m_window, GLFW_TRUE);
        }
        //
        ImGui::EndMenu();
    }
    // . View .
    if (ImGui::BeginMenu("View")) {
        if (ImGui::MenuItem("Dear ImGui demo window", nullptr, do_show_demo_window)) {
            do_show_demo_window = !do_show_demo_window;
        }
        //
        ImGui::EndMenu();
    }
    // . Help .
    if (ImGui::BeginMenu("Help")) {
        if (ImGui::MenuItem("About")) {
            is_about_popup_queued = true;
        }
        //
        ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();
    ImGui::PopFont();

    // .: Main "full-viewport" window :.
    ModuleMain();

    // .: Show the big demo window if enabled :.
    if (do_show_demo_window) {
        ImGui::PushFont(nullptr, 16.0f);
        ImGui::ShowDemoWindow(&do_show_demo_window); // Putting the bool here will add close button to the demo window
        ImGui::PopFont();
    }

    // .: About modal :.
    if (is_about_popup_queued) {
        is_about_popup_queued = false;
        ImGui::OpenPopup(c_modal_about_name);
    }
    // Always center this window when appearing
    const ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    if (ImGui::BeginPopupModal(c_modal_about_name, nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("DiagramPche :: Dear ImGui");
        ImGui::TextLinkOpenURL("github.com/RadekMocek/DiagramPche_DearImGui",
                               "https://github.com/RadekMocek/DiagramPche_DearImGui");
        ImGui::Dummy(ImVec2(0.0f, 20.0f));
        if (ImGui::Button("Close", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
        ImGui::EndPopup();
    }
}
