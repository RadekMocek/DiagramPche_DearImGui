#include "../App.hpp"

void App::GUIMainMenuBar()
{
    ImGui::PushFont(nullptr, 16.0f);
    ImGui::BeginMainMenuBar();
    // .: File :.
    if (ImGui::BeginMenu("File")) {
        // . Exit .
        if (ImGui::MenuItem("Exit", "Alt+F4")) {
            glfwSetWindowShouldClose(m_window, GLFW_TRUE);
        }
        // .::.
        ImGui::EndMenu();
    }
    // .: View :.
    if (ImGui::BeginMenu("View")) {
        // . Dear ImGui demo window .
        if (ImGui::MenuItem("Dear ImGui demo window", nullptr, m_do_show_demo_window)) {
            m_do_show_demo_window = !m_do_show_demo_window;
        }
        // .::.
        ImGui::EndMenu();
    }
    // .: Help :.
    if (ImGui::BeginMenu("Help")) {
        // .: Examples :.
        if (ImGui::BeginMenu("Examples")) {
            if (ImGui::MenuItem("Example 1")) {
                LoadSourceFromFile("./Resource/Example/Example1.toml");
            }
            // .::.
            ImGui::EndMenu();
        }
        // . About .
        if (ImGui::MenuItem("About")) {
            m_is_about_popup_queued = true;
        }
        // .::.
        ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();
    ImGui::PopFont();
}
