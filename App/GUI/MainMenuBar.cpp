#include  "../../Dependency/IconsMaterialDesignIcons.h"

#include "../App.hpp"
#include "../Config.hpp"

void App::GUIMainMenuBar()
{
    ImGui::PushFont(nullptr, FONT_SIZE_MAIN_MENU_BAR);
    ImGui::BeginMainMenuBar();
    // .: File :.
    if (ImGui::BeginMenu("File")) {
        // . New .
        if (ImGui::MenuItem(ICON_MDI_FILE_PLUS_OUTLINE" New")) {
            if (!m_is_source_dirty) {
                HandleRegularNew();
            }
            else {
                m_action_unsavedwarn_type = ActionAfterUnsavedWarn_New;
                m_is_queued_popup_unsavedwarn = true;
            }
        }
        // . Open .
        if (ImGui::MenuItem(ICON_MDI_FOLDER_OPEN_OUTLINE" Open")) {
            if (!m_is_source_dirty) {
                HandleRegularOpen();
            }
            else {
                m_action_unsavedwarn_type = ActionAfterUnsavedWarn_OpenFile;
                m_is_queued_popup_unsavedwarn = true;
            }
        }
        // . Save .
        if (ImGui::MenuItem(ICON_MDI_CONTENT_SAVE_OUTLINE" Save")) {
            HandleRegularSave();
        }
        // . Save as .
        if (ImGui::MenuItem(ICON_MDI_CONTENT_SAVE_EDIT_OUTLINE" Save as")) {
            SaveSourceToFileFromDialog();
        }
        // . Export to SVG .
        if (ImGui::MenuItem(ICON_MDI_EXPORT" Export to SVG")) {
            m_is_queued_popup_export = true;
        }
        ImGui::Separator();
        // . Preferences .
        if (ImGui::MenuItem(ICON_MDI_WRENCH_OUTLINE" Preferences", nullptr, m_do_show_window_preferences)) {
            m_do_show_window_preferences = !m_do_show_window_preferences;
        }
        ImGui::Separator();
        // . Exit .
        if (ImGui::MenuItem(ICON_MDI_EXIT_RUN" Exit", "Alt+F4")) {
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
        // . Toolbar .
        if (ImGui::MenuItem("Toolbar", nullptr, m_do_show_toolbar)) {
            m_do_show_toolbar = !m_do_show_toolbar;
        }
        ImGui::Separator();
        // . Grid .
        if (ImGui::MenuItem("Canvas grid", nullptr, m_do_show_grid)) {
            m_do_show_grid = !m_do_show_grid;
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
            // . Z-axis, out-of-order .
            if (ImGui::MenuItem("Z-axis, out-of-order")) {
                HandleOpenExample("./Resource/Example/Debug/Z-axis.toml");
            }
            // .::.
            ImGui::EndMenu();
        }
        // . Dear ImGui demo window .
#ifdef INCLUDE_IMGUI_DEMO_WINDOW
        if (ImGui::MenuItem("Dear ImGui demo window", nullptr, m_do_show_window_demo)) {
            m_do_show_window_demo = !m_do_show_window_demo;
        }
#endif
        // .::.
        ImGui::EndMenu();
    }
    // .: Help :.
    if (ImGui::BeginMenu("Help")) {
        // .: Examples :.
        if (ImGui::BeginMenu("Examples")) {
            // . Example 1 .
            if (ImGui::MenuItem("Example 1")) {
                HandleOpenExample("./Resource/Example/Example1.toml");
            }
            // . Example 2 .
            if (ImGui::MenuItem("Example 2")) {
                HandleOpenExample("./Resource/Example/Example2.toml");
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
