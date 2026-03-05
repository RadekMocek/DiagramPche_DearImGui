#include "../App.hpp"
#include "../Config.hpp"
#include "../Helper/Color.hpp"
#include "../Helper/DrawLayer.hpp"

void App::GUICanvasDrawNodes(ImDrawList* draw_list, const ImVec2 origin, const float zoom_level, const int font_size)
{
    const auto n_nodes = static_cast<int>(m_parser.m_result_nodes_pq.size());
    auto node_n = 0;

    const float node_padding = NODE_BORDER_OFFSET_BASE * zoom_level;

    for (; !m_parser.m_result_nodes_pq.empty(); m_parser.m_result_nodes_pq.pop()) {
        const auto& [node_draw_batch_number, node_id] = m_parser.m_result_nodes_pq.top();
        const auto& node = m_parser.m_result_nodes[node_id];

        // Dear ImGui text functions take `const char*`
        const auto label_c_str = node.value.c_str();

        // This gives us the size of label if we draw it; it's used for implicit (AAB)Rectangle size and for determining the label position
        const auto label_size = m_font_inconsolata_medium->
            CalcTextSizeA(static_cast<float>(font_size), FLT_MAX, -1.0f, label_c_str);

        // Get explicit or calculate implicit node size
        const auto node_width = (node.width > 0)
                                    ? static_cast<float>(node.width) * zoom_level
                                    : label_size.x + 2 * node_padding;
        const auto node_height = (node.height > 0)
                                     ? static_cast<float>(node.height) * zoom_level
                                     : label_size.y + 2 * node_padding;

        // Get node position, this is from the line `xy = [number, number]`
        const ImVec2 node_pos(static_cast<float>(node.position.x) * zoom_level,
                              static_cast<float>(node.position.y) * zoom_level);

        // Move node according to its parent, if the user had set some; this is where we use stored AABR from `canvas_nodes`
        ImVec2 parent_offset(0, 0);
        if (!node.position.parent_id.empty()) {
            if (const auto it = m_canvas_nodes.find(node.position.parent_id); it != m_canvas_nodes.end()) {
                parent_offset = it->second.GetExactPointFromPivot(node.position.parent_pivot);
            }
        }

        // Move node according to its `pivot`, if the user had set some
        ImVec2 pivot_offset(0, 0);

        switch (node.pivot) {
        case PIVOT_TOPLEFT:
            break;
        case PIVOT_TOP:
            pivot_offset.x -= node_width / 2;
            break;
        case PIVOT_TOPRIGHT:
            pivot_offset.x -= node_width;
            break;
        case PIVOT_RIGHT:
            pivot_offset.x -= node_width;
            pivot_offset.y -= node_height / 2;
            break;
        case PIVOT_BOTTOMRIGHT:
            pivot_offset.x -= node_width;
            pivot_offset.y -= node_height;
            break;
        case PIVOT_BOTTOM:
            pivot_offset.x -= node_width / 2;
            pivot_offset.y -= node_height;
            break;
        case PIVOT_BOTTOMLEFT:
            pivot_offset.y -= node_height;
            break;
        case PIVOT_LEFT:
            pivot_offset.y -= node_height / 2;
            break;
        case PIVOT_CENTER:
            pivot_offset.x -= node_width / 2;
            pivot_offset.y -= node_height / 2;
            break;
        }

        // Prepare data for storing in `m_canvas_nodes` (AABR etc.), used for drawing and canvas interaction later
        const auto aabr_top_left = node_pos + parent_offset + pivot_offset;
        const ImVec2 aabr_bottom_right(aabr_top_left.x + node_width,
                                       aabr_top_left.y + node_height);

        // ReSharper disable once CppUseStructuredBinding
        auto& canvas_node = m_canvas_nodes[node.id];
        canvas_node.top_left = aabr_top_left;
        canvas_node.bottom_right = aabr_bottom_right;
        canvas_node.center = ImVec2(aabr_top_left.x + node_width / 2,
                                    aabr_top_left.y + node_height / 2);
        canvas_node.z_mul = node.z * n_nodes + node_n++;
        canvas_node.def_line_num = node.node_source.begin.line - 1;

        // We stored proper data in `m_canvas_nodes`, now we prepare for the drawing

        // By adding origin (canvas position in window + scrolling) to AABR we get proper drawing coordinates
        const auto draw_top_left = origin + aabr_top_left;
        const auto draw_bottom_right = origin + aabr_bottom_right;

        // Prepare the label
        const auto label_left_x = draw_top_left.x + node_padding;
        const auto label_top_y = draw_top_left.y + node_padding;
        ImVec2 draw_label_position(label_left_x, label_top_y);

        const auto draw_center = origin + canvas_node.center; // Helper variable

        // Custom label position?
        if (node.width > 0 || node.height > 0) {
            // Custom width/height => `label_pos` makes sense
            switch (node.label_position) {
            case PIVOT_TOPLEFT:
                break;
            case PIVOT_TOP:
                draw_label_position = {
                    draw_center.x - label_size.x / 2,
                    label_top_y
                };
                break;
            case PIVOT_TOPRIGHT:
                draw_label_position = {
                    draw_bottom_right.x - label_size.x - node_padding,
                    label_top_y
                };
                break;
            case PIVOT_RIGHT:
                draw_label_position = {
                    draw_bottom_right.x - label_size.x - node_padding,
                    draw_center.y - label_size.y / 2
                };
                break;
            case PIVOT_BOTTOMRIGHT:
                draw_label_position = {
                    draw_bottom_right.x - label_size.x - node_padding,
                    draw_bottom_right.y - label_size.y - node_padding
                };
                break;
            case PIVOT_BOTTOM:
                draw_label_position = {
                    draw_center.x - label_size.x / 2,
                    draw_bottom_right.y - label_size.y - node_padding
                };
                break;
            case PIVOT_BOTTOMLEFT:
                draw_label_position = {
                    label_left_x,
                    draw_bottom_right.y - label_size.y - node_padding
                };
                break;
            case PIVOT_LEFT:
                draw_label_position = {
                    label_left_x,
                    draw_center.y - label_size.y / 2
                };
                break;
            case PIVOT_CENTER:
                draw_label_position = {
                    draw_center.x - label_size.x / 2,
                    draw_center.y - label_size.y / 2
                };
                break;
            }
        }

        // Do the actual drawing (and possible SVG export)

        // .: Set proper layer, colors, etc. :.
        // .:================================:.
        const auto z = DLUserChannelToRealChannel(node.z, true);
        draw_list->ChannelsSetCurrent(z);
        const auto node_imcolor = GetImU32FromColorTuple(node.color);
        constexpr auto COLOR_NODE_EDGE = IM_COL32(0, 0, 0, 255);

        // .: Draw the shape :.
        // .:================:.
        switch (node.type) {
        case NTYPE_TEXT:
            break; // Do nothing
        case NTYPE_RECTANGLE:
            // Draw rectangle and its edge
            draw_list->AddRectFilled(draw_top_left, draw_bottom_right, node_imcolor, 0, 0);
            draw_list->AddRect(draw_top_left, draw_bottom_right, COLOR_NODE_EDGE, 0, 0, zoom_level);
            // SVG rectangle
            m_exporter.AddRect(z,
                               draw_top_left.x,
                               draw_top_left.y,
                               node_width,
                               node_height,
                               node.color);
            break;
        case NTYPE_ELLIPSE:
            {
                // Ellipse radius
                const ImVec2 radius(node_width / 2.0f, node_height / 2.0f);
                // Draw ellipse and its edge
                draw_list->AddEllipseFilled(draw_center, radius, node_imcolor);
                draw_list->AddEllipse(draw_center, radius, COLOR_NODE_EDGE, 0, 0, zoom_level);
                // SVG ellipse
                m_exporter.AddEllipse(z, draw_center.x, draw_center.y, node_width, node_height, node.color);
                break;
            }
        case NTYPE_DIAMOND:
            // Diamond points
            const auto top = canvas_node.GetExactPointFromPivot(PIVOT_TOP) + origin;
            const auto right = canvas_node.GetExactPointFromPivot(PIVOT_RIGHT) + origin;
            const auto bottom = canvas_node.GetExactPointFromPivot(PIVOT_BOTTOM) + origin;
            const auto left = canvas_node.GetExactPointFromPivot(PIVOT_LEFT) + origin;
            // Draw diamond and its edge
            draw_list->AddQuadFilled(top, right, bottom, left, node_imcolor);
            draw_list->AddQuad(top, right, bottom, left, COLOR_NODE_EDGE, zoom_level);
            // SVG diamond
            m_exporter.AddDiamond(z, draw_center.x, draw_center.y, top.y, right.x, bottom.y, left.x, node.color);
            break;
        }

        // .: Draw the text :.
        // .:===============:.
        // Draw node text
        draw_list->AddText(m_font_inconsolata_medium,
                           static_cast<float>(font_size),
                           draw_label_position,
                           COLOR_NODE_EDGE,
                           label_c_str);
        // SVG node text
        m_exporter.AddText(z,
                           draw_label_position.x,
                           draw_label_position.y,
                           node.value);
    }
}
