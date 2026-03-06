#include "../App.hpp"
#include "../Helper/GUICombo.hpp"

void App::GUIWinPreferences()
{
    constexpr auto TINY_SKIP = ImVec2(3.0f, 3.0f);
    constexpr auto SMALL_SKIP = ImVec2(6.0f, 6.0f);

    const auto SameLineWithDummy = [&] {
        ImGui::SameLine();
        ImGui::Dummy(SMALL_SKIP);
        ImGui::SameLine();
    };

    ImGui::Begin("Preferences", &m_do_show_window_preferences, ImGuiWindowFlags_AlwaysAutoResize);
    // --- --- ---

    // == Appearance ==
    ImGui::SeparatorText("Appearance");
    ImGui::Text("App color theme:");
    static int dummy_int = 0;
    if (ImGui::RadioButton("Light mode", &dummy_int, AppearanceTheme_Light)) {
        ChangeAppearanceTheme(AppearanceTheme_Light);
    }
    SameLineWithDummy();
    if (ImGui::RadioButton("Dark mode", &dummy_int, AppearanceTheme_Dark)) {
        ChangeAppearanceTheme(AppearanceTheme_Dark);
    }
    SameLineWithDummy();
    if (ImGui::RadioButton("Legacy mode", &dummy_int, AppearanceTheme_Legacy)) {
        ChangeAppearanceTheme(AppearanceTheme_Legacy);
    }
    ImGui::Dummy(TINY_SKIP);
    ImGui::Text("Canvas color theme:");
    static bool dummy_bool = true;
    ImGui::Checkbox("Keep canvas light", &dummy_bool);

    // == Text editor ==
    ImGui::Dummy(SMALL_SKIP);
    ImGui::SeparatorText("Text editor");
    const char* items[] = {"Vanilla (InputTextMultiline)  ", "3rd Party (ImGuiColorTextEdit)"};
    static int item_selected_idx = m_do_use_alt_editor ? 1 : 0;

    ImGui::PushItemWidth(350);
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
    ImGui::PopItemWidth();

    // --- --- ---
    ImGui::End();
}
