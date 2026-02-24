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
}
