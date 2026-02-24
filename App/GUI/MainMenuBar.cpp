#include "../App.hpp"
#include "../Config.hpp"

void App::GUIMainMenuBar()
{
    ImGui::PushFont(nullptr, FONT_SIZE_MAIN_MENU_BAR);
    ImGui::BeginMainMenuBar();
    // .: File :.
    if (ImGui::BeginMenu("File")) {
        // . Export to SVG .
        if (ImGui::MenuItem("Export to SVG")) {
            m_is_queued_popup_export = true;
        }
        // . Exit .
        if (ImGui::MenuItem("Exit", "Alt+F4")) {
            glfwSetWindowShouldClose(m_window, GLFW_TRUE);
        }
        // .::.
        ImGui::EndMenu();
    }
    // .: View :.
    if (ImGui::BeginMenu("View")) {
        // . Grid .
        if (ImGui::MenuItem("Grid", nullptr, m_do_show_grid)) {
            m_do_show_grid = !m_do_show_grid;
        }
        // . Jump to canvas origin .
        if (ImGui::MenuItem("Jump to canvas origin")) {
            ResetCanvasScrollingAndZoom();
        }
        // .::.
        ImGui::EndMenu();
    }
    // .: Debug :.
    if (ImGui::BeginMenu("Debug")) {
        // .: Render tests :.
        if (ImGui::BeginMenu("Render tests")) {
            if (ImGui::MenuItem("Z-axis, out-of-order")) {
                LoadSourceFromFile("./Resource/Example/Debug/Z-axis.toml");
            }
            // .::.
            ImGui::EndMenu();
        }
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
            m_is_queued_popup_about = true;
        }
        // .::.
        ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();
    ImGui::PopFont();
}
