#include "../App.hpp"
#include "../Helper/GUICombo.hpp"

void App::GUIWinPreferences()
{
    ImGui::Begin("Preferences", &m_do_show_window_preferences, ImGuiWindowFlags_AlwaysAutoResize);
    // --- --- ---

    ImGui::SeparatorText("Text editor");
    const char* items[] = {"Vanilla (InputTextMultiline)  ", "3rd Party (ImGuiColorTextEdit)"};
    static int item_selected_idx = m_do_use_alt_editor ? 1 : 0;

    if (GUICombo("preferred editor", items, IM_COUNTOF(items), item_selected_idx)) {
        const bool do_use_alt_editor = item_selected_idx == 1;
        if (do_use_alt_editor) {
            m_alt_editor.SetText(m_source);
        }
        /*else {
            m_source = m_alt_editor.GetText(); // This gets called every frame so shouldn't be needed to call here as well
        }*/
        m_do_use_alt_editor = do_use_alt_editor;
    }

    // --- --- ---
    ImGui::End();
}
