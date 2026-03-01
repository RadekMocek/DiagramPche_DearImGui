#include "App.hpp"

void App::Update()
{
    // .: Main menu bar :.
    GUIMainMenuBar();

    // .: Main "full-viewport" window :.
    GUIBody();

    // .: Show the big demo window if enabled :.
    if (m_do_show_window_demo) {
        ImGui::PushFont(nullptr, 16.0f);
        ImGui::ShowDemoWindow(&m_do_show_window_demo); // Putting the bool here will add close button to the demo window
        ImGui::PopFont();
    }

    // Must be called before modals because Export modal starts the exporter
    if (m_exporter.IsEnabled()) {
        if (m_exporter.Save()) {
            if (m_action_after_export_choice == ActionAfterExport_OpenFolder) {
                ShowFileInFileManager(m_path_export);
            }
            else if (m_action_after_export_choice == ActionAfterExport_OpenFile) {
                OpenFile(m_path_export);
            }
        }
        else {
            ShowErrorModal(
                "SVG file could not be created.\nMaybe the specified path contained some non-existing directories or forbidden characters?");
        }
    }

    // .: Modeless windows :.
    if (m_do_show_window_preferences) GUIWinPreferences();

    // .: Modals :.
    GUIModal();

    // Post modal actions
    if (m_is_action_unsavedwarn_queued) {
        m_is_action_unsavedwarn_queued = false;
        if (m_do_action_unsavedwarn_save) {
            HandleRegularSave();
        }
        switch (m_action_unsavedwarn_type) {
        case ActionAfterUnsavedWarn_Invalid:
            std::cerr << "ActionAfterUnsavedWarn_Invalid\n";
            break;
        case ActionAfterUnsavedWarn_Exit:
            m_should_window_really_close = true;
            glfwSetWindowShouldClose(m_window, GLFW_TRUE);
            break;
        case ActionAfterUnsavedWarn_New:
            HandleRegularNew();
            break;
        case ActionAfterUnsavedWarn_OpenFile:
            HandleRegularOpen();
            break;
        case ActionAfterUnsavedWarn_LoadExample:
            LoadSourceFromFile(m_action_unsavedwarn_value.c_str(), true);
            break;
        }
        m_action_unsavedwarn_type = ActionAfterUnsavedWarn_Invalid;
    }

    // Update window title
    static std::string window_title;
    window_title.clear();
    if (m_is_source_dirty) {
        window_title.push_back('*');
    }
    window_title += m_source_filename.value_or("Untitled");
    window_title += " – DiagramPche :: Dear ImGui";
    SetWindowTitle(window_title.c_str());
}
