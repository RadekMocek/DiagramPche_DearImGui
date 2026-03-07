#include "../App.hpp"
#include "../Config.hpp"
#include "../Helper/GUICombo.hpp"
#include "../Helper/GUILayout.hpp"

void App::GUIWinPreferences()
{
    ImGui::Begin("Preferences", &m_do_show_window_preferences, ImGuiWindowFlags_AlwaysAutoResize);
    // --- --- --- ---
    if (ImGui::BeginTabBar("PreferencesTabBar")) {
        // == Appearance ==
        if (ImGui::BeginTabItem("Appearance")) {
            ImGui::Dummy(TINY_SKIP);
            ImGui::SeparatorText("App color theme");
            static int color_theme_int = 0;
            if (ImGui::RadioButton("Light mode", &color_theme_int, AppearanceTheme_Light)) {
                ChangeAppearanceTheme(AppearanceTheme_Light);
            }
            SameLineWithDummy();
            if (ImGui::RadioButton("Dark mode", &color_theme_int, AppearanceTheme_Dark)) {
                ChangeAppearanceTheme(AppearanceTheme_Dark);
            }
            SameLineWithDummy();
            if (ImGui::RadioButton("Legacy mode", &color_theme_int, AppearanceTheme_Legacy)) {
                ChangeAppearanceTheme(AppearanceTheme_Legacy);
            }
            ImGui::Dummy(SMALL_SKIP);
            ImGui::SeparatorText("Canvas color theme");
            ImGui::Checkbox("Keep canvas light", &m_style_do_force_light_canvas);
            // --- --- --- --- ---
            ImGui::EndTabItem();
        }
        // == Text editor ==
        if (ImGui::BeginTabItem("Text editor")) {
            ImGui::Dummy(TINY_SKIP);
            ImGui::SeparatorText("Text editor font size");
            ImGui::InputInt("##FontSizeInputInt", &m_source_font_size, 2);
            if (m_source_font_size < FONT_SIZE_SOURCE_MIN) { m_source_font_size = FONT_SIZE_SOURCE_MIN; }
            else if (m_source_font_size > FONT_SIZE_SOURCE_MAX) { m_source_font_size = FONT_SIZE_SOURCE_MAX; }

            ImGui::Dummy(SMALL_SKIP);
            ImGui::SeparatorText("Preferred text editor");
            const char* items[] = {"Vanilla (InputTextMultiline)  ", "3rd Party (ImGuiColorTextEdit)"};
            static int item_selected_idx = m_do_use_alt_editor ? 1 : 0;

            ImGui::PushItemWidth(350);
            if (GUICombo("##preferred editor", items, IM_COUNTOF(items), item_selected_idx)) {
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
            // --- --- --- --- ---
            ImGui::EndTabItem();
        }
        // == View ==
        if (ImGui::BeginTabItem("View")) {
            ImGui::Dummy(TINY_SKIP);
            ImGui::Checkbox("Toolbar", &m_do_show_toolbar);
            ImGui::Checkbox("Canvas grid", &m_do_show_grid);
            ImGui::Checkbox("Secondary canvas toolbar", &m_do_show_secondary_canvas_toolbar);
            // --- --- --- --- ---
            ImGui::EndTabItem();
        }
        // --- --- --- --- ---
        ImGui::EndTabBar();
    }
    /*
    ImGui::Dummy(BIG_SKIP);
    ImGui::Button("Save");
    ImGui::SameLine();
    HelpMarker(
        "This will save ALL settings to disk."
        "Your unsaved preferences will only be valid for the current session."
    );
    */
    // --- --- --- ---
    ImGui::End();
}
