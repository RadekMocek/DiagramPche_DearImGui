#include "../App.hpp"

void App::GUIWinPreferences()
{
    ImGui::Begin("Preferences", &m_do_show_window_preferences, ImGuiWindowFlags_AlwaysAutoResize);

    ImGui::SeparatorText("Text editor");
    static int text_editor_selection = m_do_use_alt_editor ? 1 : 0;
    if (ImGui::Combo("preferred editor", &text_editor_selection,
                     "Vanilla (InputTextMultiline) \0"
                     "3rd Party (ImGuiColorTextEdit)\0\0")) {
        const bool do_use_alt_editor = text_editor_selection == 1;
        if (do_use_alt_editor) {
            m_alt_editor.SetText(m_source);
        }
        else {
            m_source = m_alt_editor.GetText();
        }
        m_do_use_alt_editor = do_use_alt_editor;
    }
    ImGui::End();
}
