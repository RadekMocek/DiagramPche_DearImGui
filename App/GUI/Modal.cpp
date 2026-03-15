#include <filesystem>

#include  "../../Dependency/IconsMaterialDesignIcons.h"
#include "misc/cpp/imgui_stdlib.h"

#include "../App.hpp"
#include "../Config.hpp"
#include "../Helper/Color.hpp"
#include "../Helper/GUILayout.hpp"

void App::GUIModal()
{
    constexpr auto FLAGS_AUTOSIZE_AND_NOINI = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings;

    constexpr auto COLOR_GOOD_NORMAL = IM_COL32(114, 224, 59, 102);
    constexpr auto COLOR_GOOD_HOVER = IM_COL32(114, 224, 59, 255);
    constexpr auto COLOR_GOOD_CLICK = IM_COL32(84, 224, 13, 255);
    constexpr auto COLOR_BAD_NORMAL = IM_COL32(224, 59, 59, 102);
    constexpr auto COLOR_BAD_HOVER = IM_COL32(224, 59, 59, 255);
    constexpr auto COLOR_BAD_CLICK = IM_COL32(224, 13, 13, 255);

    // (For centering modals when they appear)
    const auto CenterNextModal = [] {
        const ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    };

    // (Make modal bg color same as normal window color)
    ImGui::PushStyleColor(ImGuiCol_PopupBg, m_style_color_modal);

    // .: Export modal :.
    // .:==============:.
    CenterNextModal();
    constexpr auto MODAL_EXPORT_NAME = "Export to SVG##modal";
    static bool _is_popup_export_open = true; // To enable modal close button
    static bool do_overwrite_export;
    static bool is_overwrite_export_needed;
    static bool can_export = true;
    if (m_is_queued_popup_export) {
        m_is_queued_popup_export = false;
        _is_popup_export_open = true;
        do_overwrite_export = false;
        ImGui::OpenPopup(MODAL_EXPORT_NAME);
    }
    if (ImGui::BeginPopupModal(MODAL_EXPORT_NAME, &_is_popup_export_open, FLAGS_AUTOSIZE_AND_NOINI)) {
        ImGui::SeparatorText("Location");
        if (ImGui::InputTextWithHint("##path to SVG", "input path to SVG here", &m_path_export,
                                     ImGuiInputTextFlags_ElideLeft)) {
            do_overwrite_export = false;
        }
        ImGui::SameLine();
        if (ImGui::Button("Browse...")) {
            if (const auto dialog_result = SaveSVGDialog(); dialog_result.has_value()) {
                m_path_export = dialog_result.value();
                do_overwrite_export = false;
            }
        }

        ImGui::Dummy(SMALL_SKIP);
        ImGui::SeparatorText("Overwrite guard");
        is_overwrite_export_needed = false;
        if (std::filesystem::exists(m_path_export)) {
            can_export = false;
            ImGui::PushStyleColor(ImGuiCol_Text, COLOR_ERROR);
            if (!std::filesystem::is_directory(m_path_export)) {
                ImGui::Text("File at the specified path already exists.");
                is_overwrite_export_needed = true;
                if (do_overwrite_export) {
                    can_export = true;
                }
            }
            else {
                ImGui::Text("The specified path is a directory.");
            }
            ImGui::PopStyleColor();
        }
        else {
            ImGui::Text("Specified path is unique.");
            can_export = true;
        }
        ImGui::BeginDisabled(!is_overwrite_export_needed);
        ImGui::Checkbox("Overwrite", &do_overwrite_export);
        ImGui::EndDisabled();

        ImGui::Dummy(SMALL_SKIP);
        ImGui::SeparatorText("Action after export");
        ImGui::RadioButton("Nothing", &m_action_after_export_choice, ActionAfterExport_DoNothing);
        SameLineWithDummy(SMALL_SKIP);
        ImGui::RadioButton("Show in explorer", &m_action_after_export_choice, ActionAfterExport_OpenFolder);
        SameLineWithDummy(SMALL_SKIP);
        ImGui::RadioButton("Open", &m_action_after_export_choice, ActionAfterExport_OpenFile);

#ifndef _WIN32
        ImGui::Text((m_action_after_export_choice == ActionAfterExport_DoNothing)
                        ? ""
                        : "Package `xdg-utils` will be used for that.");
#endif

        ImGui::Dummy(BIG_SKIP);
        ImGui::BeginDisabled(!can_export);
        if (ImGui::Button("Export")) {
            m_exporter.Start(m_path_export);
            // SVG export starts here, but the SVG will be created along with the canvas drawing in the next loop.
            // So the function to actually save the SVG and do action after export is located before this code (see `AppUpdate.cpp`).
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndDisabled();

        // --- --- --- ---
        ImGui::EndPopup();
    }

    // .: About modal :.
    // .:=============:.
    CenterNextModal();
    constexpr auto MODAL_ABOUT_NAME = "About##modal";
    if (m_is_queued_popup_about) {
        m_is_queued_popup_about = false;
        ImGui::OpenPopup(MODAL_ABOUT_NAME);
    }
    if (ImGui::BeginPopupModal(MODAL_ABOUT_NAME, nullptr, FLAGS_AUTOSIZE_AND_NOINI)) {
        ImGui::Text("DiagramPche :: Dear ImGui");
        ImGui::TextLinkOpenURL("github.com/RadekMocek/DiagramPche_DearImGui",
                               "https://github.com/RadekMocek/DiagramPche_DearImGui");
        ImGui::Dummy(BIG_SKIP);
        if (ImGui::Button("Close", BUTTON_WIDER)) { ImGui::CloseCurrentPopup(); }
        // --- --- --- ---
        ImGui::EndPopup();
    }

    // .: Unsaved file warning modal :.
    // .:============================:.
    CenterNextModal();
    constexpr auto MODAL_UNSAVEDWARN_NAME = "You have unsaved changes##modal";
    if (m_is_queued_popup_unsavedwarn) {
        m_is_queued_popup_unsavedwarn = false;
        ImGui::OpenPopup(MODAL_UNSAVEDWARN_NAME);
    }
    if (ImGui::BeginPopupModal(MODAL_UNSAVEDWARN_NAME, nullptr, FLAGS_AUTOSIZE_AND_NOINI)) {
        ImGui::Text("Do you want to save changes to\n'%s'?", m_source_filename.value_or("Untitled").c_str());
        ImGui::Dummy(BIG_SKIP);

        SetButtonColors(COLOR_GOOD_NORMAL, COLOR_GOOD_HOVER, COLOR_GOOD_CLICK);
        if (ImGui::Button(ICON_MDI_CONTENT_SAVE_OUTLINE" Save", BUTTON_WIDER)) {
            m_is_action_unsavedwarn_queued = true;
            m_do_action_unsavedwarn_save = true;
            ImGui::CloseCurrentPopup();
        }
        ResetButtonColors();

        ImGui::SameLine();
        SetButtonColors(COLOR_BAD_NORMAL, COLOR_BAD_HOVER, COLOR_BAD_CLICK);
        if (ImGui::Button(ICON_MDI_TRASH_CAN_OUTLINE" Discard", BUTTON_WIDER)) {
            m_is_action_unsavedwarn_queued = true;
            m_do_action_unsavedwarn_save = false;
            ImGui::CloseCurrentPopup();
        }
        ResetButtonColors();

        ImGui::SameLine();
        if (ImGui::Button(ICON_MDI_CANCEL" Cancel", BUTTON_WIDER)) { ImGui::CloseCurrentPopup(); }
        // --- --- --- ---
        ImGui::EndPopup();
    }

    // .: Error modal :.
    // .:=============:.
    CenterNextModal();
    constexpr auto MODAL_ERROR_NAME = "Error##modal";
    if (m_is_queued_popup_error) {
        m_is_queued_popup_error = false;
        ImGui::OpenPopup(MODAL_ERROR_NAME);
    }
    if (ImGui::BeginPopupModal(MODAL_ERROR_NAME, nullptr, FLAGS_AUTOSIZE_AND_NOINI)) {
        ImGui::PushStyleColor(ImGuiCol_Text, COLOR_ERROR);
        ImGui::Text("%s", m_modal_error_message.c_str());
        ImGui::PopStyleColor();
        ImGui::Dummy(BIG_SKIP);
        if (ImGui::Button("RIP", BUTTON_WIDER)) { ImGui::CloseCurrentPopup(); }
        // --- --- --- ---
        ImGui::EndPopup();
    }

    // --- --- --- --- ---
    ImGui::PopStyleColor();
}
