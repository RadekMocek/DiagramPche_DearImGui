#include "../App.hpp"
#include "../Config.hpp"

void App::GUITextEditorAlt(const float textedit_width)
{
    //*
    const ImVec2 textedit_size(textedit_width, ImGui::GetContentRegionAvail().y - BOTTOM_BAR_HEIGHT);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    constexpr auto flags = ImGuiChildFlags_Borders;
    ImGui::BeginChild("SourceParent", textedit_size, flags);
    ImGui::PopStyleVar();
    /**/
    // --- --- --- --- --- --- --- ---

    m_alt_editor.Render("##SourceAlt");
    m_source = m_alt_editor.GetText();

    if (m_parser.m_is_error) {
        TextEditor::ErrorMarkers markers;
        markers.insert(std::make_pair<int, std::string>(
            m_parser.m_error_source_region.begin.line,
            m_parser.m_error_description.c_str()
        ));
        m_alt_editor.SetErrorMarkers(markers);
    }

    // --- --- --- --- --- --- --- ---
    //*
    ImGui::EndChild();
    /**/
}
