#include "../App.hpp"
#include "../Config.hpp"

void App::GUITextEditorAlt(const float textedit_width)
{
    const ImVec2 textedit_size(textedit_width, ImGui::GetContentRegionAvail().y - BOTTOM_BAR_HEIGHT);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    constexpr auto flags = ImGuiChildFlags_Borders;
    ImGui::BeginChild("SourceParent", textedit_size, flags);
    ImGui::PopStyleVar();
    // --- --- --- --- --- --- --- ---

    m_alt_editor.Render("##SourceAlt");
    m_source = m_alt_editor.GetText(); // Diagram gets build from m_source

    if (!m_source.empty() && m_source.back() == '\n') {
        m_source.pop_back();
    }

    if (m_alt_editor.IsTextChanged()) {
        m_is_source_dirty = true;
    }

    m_alt_editor.GetErrorMarkersRef().clear();
    if (m_parser.m_is_error) {
        m_alt_editor.GetErrorMarkersRef().insert(std::make_pair<int, std::string>(
            static_cast<int>(m_parser.m_error_source_region.begin.line),
            "" // Error highlight tooltip is turned off so no need to set this
        ));
    }

    // --- --- --- --- --- --- --- ---
    ImGui::EndChild();
}
