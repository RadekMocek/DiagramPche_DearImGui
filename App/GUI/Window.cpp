#include "../App.hpp"
#include "../Config.hpp"
#include "../Helper/GUICombo.hpp"
#include "../Helper/GUILayout.hpp"

void App::GUIWinPreferences()
{
    ImGui::SetNextWindowSizeConstraints(ImVec2(450, 205), ImVec2(FLT_MAX, FLT_MAX));
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
            SameLineWithDummy(SMALL_SKIP);
            if (ImGui::RadioButton("Dark mode", &color_theme_int, AppearanceTheme_Dark)) {
                ChangeAppearanceTheme(AppearanceTheme_Dark);
            }
            SameLineWithDummy(SMALL_SKIP);
            if (ImGui::RadioButton("Legacy mode", &color_theme_int, AppearanceTheme_Legacy)) {
                ChangeAppearanceTheme(AppearanceTheme_Legacy);
            }
            ImGui::Dummy(SMALL_SKIP);
            ImGui::SeparatorText("Canvas color theme");
            ImGui::BeginDisabled(color_theme_int == 0);
            ImGui::Checkbox("Keep canvas light", &m_style_do_force_light_canvas);
            ImGui::EndDisabled();
            // --- --- --- --- ---
            ImGui::EndTabItem();
        }
        // == Text editor ==
        if (ImGui::BeginTabItem("Text editor")) {
            ImGui::Dummy(TINY_SKIP);
            ImGui::SeparatorText("Text editor font size");
            WidgetTextEditorFontSizeInputInt();

            ImGui::Dummy(SMALL_SKIP);
            ImGui::SeparatorText("Preferred text editor");
            WidgetTextEditorPreferredCombo();
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
    //std::cout << ImGui::GetWindowSize() << "\n";
    ImGui::End();
}

// This method handles the benchmark window only before the user starts the benchmark. Everything else benchmark related is in a special file `Logic→AppBenchmark.cpp`
void App::GUIWinBenchmark()
{
    ImGuiWindowFlags flags = ImGuiWindowFlags_AlwaysAutoResize;
    if (m_is_benchmark_running) {
        flags |= ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDecoration;
        constexpr auto WINDOW_OFFSET = 12;
        ImGui::SetNextWindowPos({WINDOW_OFFSET, WINDOW_OFFSET});
    }

    ImGui::Begin("Benchmark", &m_do_show_window_benchmark, flags);
    // --- --- --- ---
    if (!m_is_benchmark_running) {
        ImGui::Text("Syntax highlight may affect performance:");
        WidgetTextEditorPreferredCombo();
        ImGui::Dummy(TINY_SKIP);

        ImGui::Text("Choose one of the three benchmarks:");
        static int item_selected_idx = 0;
        GUICombo("##BenchmarkTypeCombo", BENCHMARK_TYPE_NAMES, item_selected_idx);

        ImGui::Dummy(TINY_SKIP);
        ImGui::Separator();
        ImGui::Dummy(TINY_SKIP);

        if (m_is_source_dirty) {
            ImGui::PushStyleColor(ImGuiCol_Text, COLOR_ERROR);
            ImGui::Text("You have unsaved changes, save your work before running the benchmark.");
            ImGui::PopStyleColor();
            ImGui::Text("(If you don't wish to save this, select File → New → Discard.)");
        }
        else {
            if (ImGui::Button("Start benchmark")) {
                BenchmarkStart(static_cast<BenchmarkType>(item_selected_idx));
            }
        }
    }
    else {
        BenchmarkGUIUpdate();
    }
    // --- --- --- ---
    //std::cout << ImGui::GetWindowSize() << "\n";
    ImGui::End();
}
