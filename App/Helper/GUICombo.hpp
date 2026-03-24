#pragma once

#include "imgui.h"

template <std::size_t SIZE>
bool GUICombo(const char* label, const std::array<const char*, SIZE>& items, int& item_selected_idx,
              const ImGuiComboFlags flags = 0)
{
    auto result = false; // Has value changed?

    if (const char* combo_preview_value = items[item_selected_idx];
        ImGui::BeginCombo(label, combo_preview_value, flags)) {
        for (int n = 0; n < SIZE; n++) {
            const bool is_selected = item_selected_idx == n;
            if (ImGui::Selectable(items[n], is_selected)) {
                result = item_selected_idx != n;
                item_selected_idx = n;
            }
            if (is_selected) {
                ImGui::SetItemDefaultFocus();
            }
        }

        ImGui::EndCombo();
    }

    return result;
}
