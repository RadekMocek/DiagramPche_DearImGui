#include <algorithm>

#include "../App.hpp"
#include "../Helper/Color.hpp"

void App::GUIToolbar()
{
    constexpr auto DEFAULT_COLOR = ImVec4(114.0f / 255.0f, 144.0f / 255.0f, 154.0f / 255.0f, 200.0f / 255.0f);

    static std::string node_key_label_value;
    static ImVec4 color;
    static std::optional<toml::source_region> color_source;

    if (m_selected_or_hovered_canvas_node_key.has_value()
        && m_canvas_nodes.contains(m_selected_or_hovered_canvas_node_key.value())) {
        // Case: some node in canvas is selected/hovered and exists (we have info about it in `m_canvas_nodes` (with one frame delay, it is what it is))
        const auto& node = m_canvas_nodes[m_selected_or_hovered_canvas_node_key.value()];
        // TODO I am copying here, not ideal, rethink this after I rethinking the TOML parser pq situation
        color = GetImVec4FromColorTuple(node.color);
        color_source = node.color_source;
        // Update `node_key_label_value` accordingly
        constexpr auto NODE_KEY_LENGTH = 18;
        node_key_label_value = m_selected_or_hovered_canvas_node_key.value().substr(0, NODE_KEY_LENGTH);
        node_key_label_value.resize(NODE_KEY_LENGTH, ' ');
        std::ranges::replace(node_key_label_value, '\n', ' ');
    }
    else {
        // Case: nothing is selected/hovered or something was selected but then the id was changed/deleted in source
        color = DEFAULT_COLOR;
        // In case when the id does not exist anymore
        m_is_canvas_node_selected = false;
        m_selected_or_hovered_canvas_node_key = std::nullopt;
        // Update `node_key_label_value` accordingly
        constexpr auto NODE_KEY_DEFAULT_STRING = "(No node selected)";
        node_key_label_value = NODE_KEY_DEFAULT_STRING;
    }

    constexpr auto TOOLBAR_HEIGHT = 30.0f;
    const ImVec2 toolbar_size(ImGui::GetContentRegionAvail().x, TOOLBAR_HEIGHT);

    ImGui::BeginDisabled(!m_is_canvas_node_selected);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(6.0f, 2.0f));
    ImGui::BeginChild("CanvasBarParent",
                      toolbar_size,
                      ImGuiChildFlags_Borders,
                      ImGuiWindowFlags_None);
    ImGui::PopStyleVar();

    ImGui::AlignTextToFramePadding();
    ImGui::Text("%s", node_key_label_value.c_str());

    ImGui::SameLine();
    if (ImGui::ColorEdit4("Node color", reinterpret_cast<float*>(&color),
                          ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel)) {
        if (color_source.has_value()) {
            //TODO do not change when color is the same?

            const auto start_opt = GetMSourceIdxFromSourceRegion(color_source.value().begin);
            const auto end_opt = GetMSourceIdxFromSourceRegion(color_source.value().end);
            if (start_opt.has_value() && end_opt.has_value()) {
                const auto start = start_opt.value() - 1;
                const auto end = end_opt.value() - 1;

                std::cout << "replacing: '" << m_source.substr(start, end - start) << "'\n";
                std::cout << "start=" << start << " end=" << end << " len=" << (end - start) << "\n";

                const auto length = end - start;
                const auto new_str = GetRGBAHexFromImVec4(color); // TODO get this from IMGUI somehow?
                m_source.replace(start, length, std::format("\"{}\"", new_str));
            }
        }
        else {
            // TODO color undefined, add one to TOML
        }

        if (m_do_use_alt_editor) {
            m_alt_editor.SetText(m_source);
        }
    }

    //*
    ImGui::SameLine();
    ImGui::Button("Button :)");

    ImGui::SameLine();
    ImGui::AlignTextToFramePadding();
    if (color_source.has_value()) {
        ImGui::Text("(cursor %i, %i) (start %i, %i) (end %i, %i)", m_alt_editor.GetCursorPosition().mLine,
                    m_alt_editor.GetCursorPosition().mColumn, color_source.value().begin.line,
                    color_source.value().begin.column, color_source.value().end.line, color_source.value().end.column
        );

        const auto start = GetMSourceIdxFromSourceRegion(color_source.value().begin);
        const auto end = GetMSourceIdxFromSourceRegion(color_source.value().end);
        if (start.has_value() && end.has_value()) {
            const auto length = end.value() - start.value();
            ImGui::SameLine();
            ImGui::AlignTextToFramePadding();
            ImGui::Text("%i - %i = %i", end.value(), start.value(), length);
        }
    }
    else {
        ImGui::Text("Zagmolol Bool");
    }
    /**/
    // --- --- --- --- ---
    ImGui::EndDisabled();
    ImGui::EndChild();
}
