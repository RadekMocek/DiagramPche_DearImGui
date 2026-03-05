#include "../App.hpp"
#include "../Config.hpp"

void App::GUIMainMenuBar()
{
    ImGui::PushFont(nullptr, FONT_SIZE_MAIN_MENU_BAR);
    ImGui::BeginMainMenuBar();
    // .: File :.
    if (ImGui::BeginMenu("File")) {
        // . New .
        if (ImGui::MenuItem("New")) {
            if (!m_is_source_dirty) {
                HandleRegularNew();
            }
            else {
                m_action_unsavedwarn_type = ActionAfterUnsavedWarn_New;
                m_is_queued_popup_unsavedwarn = true;
            }
        }
        // . Open .
        if (ImGui::MenuItem("Open")) {
            if (!m_is_source_dirty) {
                HandleRegularOpen();
            }
            else {
                m_action_unsavedwarn_type = ActionAfterUnsavedWarn_OpenFile;
                m_is_queued_popup_unsavedwarn = true;
            }
        }
        // . Save .
        if (ImGui::MenuItem("Save")) {
            HandleRegularSave();
        }
        // . Save as .
        if (ImGui::MenuItem("Save as")) {
            SaveSourceToFileFromDialog();
        }
        // . Export to SVG .
        if (ImGui::MenuItem("Export to SVG")) {
            m_is_queued_popup_export = true;
        }
        ImGui::Separator();
        // . Preferences .
        if (ImGui::MenuItem("Preferences", nullptr, m_do_show_window_preferences)) {
            m_do_show_window_preferences = !m_do_show_window_preferences;
        }
        ImGui::Separator();
        // . Exit .
        if (ImGui::MenuItem("Exit", "Alt+F4")) {
            if (!m_is_source_dirty) {
                glfwSetWindowShouldClose(m_window, GLFW_TRUE);
            }
            else {
                m_action_unsavedwarn_type = ActionAfterUnsavedWarn_Exit;
                m_is_queued_popup_unsavedwarn = true;
            }
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
        // . Toolbar .
        if (ImGui::MenuItem("Toolbar", nullptr, m_do_show_toolbar)) {
            m_do_show_toolbar = !m_do_show_toolbar;
        }
        // . Secondary canvas toolbar .
        if (ImGui::MenuItem("Secondary canvas toolbar", nullptr, m_do_show_secondary_canvas_toolbar)) {
            m_do_show_secondary_canvas_toolbar = !m_do_show_secondary_canvas_toolbar;
        }
        // . Jump to canvas origin .
        ImGui::Separator();
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
                if (!m_is_source_dirty) {
                    LoadSourceFromFile("./Resource/Example/Debug/Z-axis.toml", true);
                }
                else {
                    m_action_unsavedwarn_type = ActionAfterUnsavedWarn_LoadExample;
                    m_action_unsavedwarn_value = "./Resource/Example/Debug/Z-axis.toml";
                    m_is_queued_popup_unsavedwarn = true;
                }
            }
            // .::.
            ImGui::EndMenu();
        }
        // . Dear ImGui demo window .
        if (ImGui::MenuItem("Dear ImGui demo window", nullptr, m_do_show_window_demo)) {
            m_do_show_window_demo = !m_do_show_window_demo;
        }
        /*
        // . For testing purposes .
        if (ImGui::MenuItem("(test)")) {
            m_is_queued_popup_unsavedwarn = true;
        }
        */
        // .::.
        ImGui::EndMenu();
    }
    // .: Help :.
    if (ImGui::BeginMenu("Help")) {
        // .: Examples :.
        if (ImGui::BeginMenu("Examples")) {
            if (ImGui::MenuItem("Example 1")) {
                if (!m_is_source_dirty) {
                    LoadSourceFromFile("./Resource/Example/Example1.toml", true);
                }
                else {
                    m_action_unsavedwarn_type = ActionAfterUnsavedWarn_LoadExample;
                    m_action_unsavedwarn_value = "./Resource/Example/Example1.toml";
                    m_is_queued_popup_unsavedwarn = true;
                }
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
