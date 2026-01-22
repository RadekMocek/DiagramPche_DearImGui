#include "../App.hpp"

void App::ModuleMainMenuBar()
{
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
        if (ImGui::MenuItem("Dear ImGui demo window", nullptr, m_do_show_demo_window)) {
            m_do_show_demo_window = !m_do_show_demo_window;
        }
        //
        ImGui::EndMenu();
    }
    // . Help .
    if (ImGui::BeginMenu("Help")) {
        if (ImGui::MenuItem("About")) {
            m_is_about_popup_queued = true;
        }
        //
        ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();
    ImGui::PopFont();
}
