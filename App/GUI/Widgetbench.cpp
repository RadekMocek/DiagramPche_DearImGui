#include "../App.hpp"
#include "../Config.hpp"

constexpr auto N_BATCH_LABELS = 20;
constexpr auto N_BATCH_TEXTEDITS = 20;
constexpr auto N_BATCH_BUTTONS = 20;
constexpr auto N_BATCH_CHECKBOXES = 48;
constexpr auto N_BATCH_RADIOS = 48;
constexpr auto N_BATCH_SLIDERS = 16;
//
constexpr auto N_BATCH_WIDGETS
    = N_BATCH_LABELS
    + N_BATCH_TEXTEDITS
    + N_BATCH_BUTTONS
    + N_BATCH_CHECKBOXES
    + N_BATCH_RADIOS
    + N_BATCH_SLIDERS;
//

void App::GUIWinWidgetbench()
{
    if (m_WB_do_show_window) {
        constexpr ImGuiWindowFlags flags
            = ImGuiWindowFlags_AlwaysAutoResize
            | ImGuiWindowFlags_NoMove
            | ImGuiWindowFlags_NoCollapse;

        const auto window_name = std::format("Widget spree {} x {}", m_WB_n_batches, N_BATCH_WIDGETS);

        ImGui::SetNextWindowPos({0, 0});
        ImGui::Begin(window_name.c_str(), nullptr, flags);

        if (!m_WB_is_running) {
            ImGui::PushStyleColor(ImGuiCol_Text, COLOR_ERROR);
            ImGui::Text("This benchmark is run from terminal: '.\\DiagramPche_DearImGui.exe w'");
            ImGui::Text("[!] epilepsy warning: this window will flicker during the benchmark");
            ImGui::PopStyleColor();
            ImGui::SameLine();
            if (ImGui::Button("Close")) {
                m_WB_do_show_window = false;
            }
        }

        const auto RevokeSameLine = [] {
            ImGui::Dummy({0, 0});
        };

        int id = 0;

        for (int b = 0; b < m_WB_n_batches; b++) {
            RevokeSameLine();

            for (int i = 0; i < N_BATCH_LABELS; i++) {
                ImGui::Text("ABCČDĎEF");
                ImGui::SameLine();
            }
            RevokeSameLine();

            static char textedittext[8] = "GHCHIJK";
            ImGui::PushItemWidth(80);
            for (int i = 0; i < N_BATCH_TEXTEDITS; i++) {
                ImGui::PushID(id++);
                ImGui::InputText("##textedit", textedittext, IM_ARRAYSIZE(textedittext));
                ImGui::PopID();
                ImGui::SameLine();
            }
            ImGui::PopItemWidth();
            RevokeSameLine();

            for (int i = 0; i < N_BATCH_BUTTONS; i++) {
                ImGui::PushID(id++);
                ImGui::Button("LMNŇOPQ");
                ImGui::PopID();
                ImGui::SameLine();
            }
            RevokeSameLine();

            ImGui::PushFont(nullptr, 14);

            static bool check = true;
            for (int i = 0; i < N_BATCH_CHECKBOXES; i++) {
                ImGui::PushID(id++);
                ImGui::Checkbox("R", &check);
                ImGui::PopID();
                ImGui::SameLine();
            }
            RevokeSameLine();

            static int radioval = 0;
            for (int i = 0; i < N_BATCH_RADIOS; i++) {
                ImGui::PushID(id++);
                ImGui::RadioButton("Ř", &radioval, 0);
                ImGui::PopID();
                ImGui::SameLine();
            }
            RevokeSameLine();

            ImGui::PopFont();

            static int sliderval = 5;
            ImGui::PushItemWidth(105);
            for (int i = 0; i < N_BATCH_SLIDERS; i++) {
                ImGui::PushID(id++);
                ImGui::SliderInt("##slider", &sliderval, 0, 9);
                ImGui::PopID();
                ImGui::SameLine();
            }
            ImGui::PopItemWidth();
        }

        // --- --- ---
        ImGui::End();
    }
}
