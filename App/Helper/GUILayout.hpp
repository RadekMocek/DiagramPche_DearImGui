#pragma once

#include "imgui.h"
#include  "../../Dependency/IconsMaterialDesignIcons.h"

constexpr auto NANO_SKIP = ImVec2(2.0f, 2.0f);
constexpr auto TINY_SKIP = ImVec2(3.0f, 3.0f);
constexpr auto SMALL_SKIP = ImVec2(6.0f, 6.0f);
constexpr auto BIG_SKIP = ImVec2(20.0f, 20.0f);
constexpr auto BUTTON_WIDER = ImVec2(120.0f, 0.0f);

inline void SameLineWithDummy(const ImVec2 dummy_size)
{
    ImGui::SameLine();
    ImGui::Dummy(dummy_size);
    ImGui::SameLine();
}

inline void HelpMarker(const char* desc)
{
    ImGui::TextDisabled(ICON_MDI_HELP_CIRCLE_OUTLINE);
    if (ImGui::BeginItemTooltip()) {
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}
