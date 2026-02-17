#include "../App.hpp"
#include "../Config.hpp"
#include "../Helper/Color.hpp"
#include "../Helper/Draw.hpp"
#include "../Helper/DrawLayer.hpp"

void App::GUICanvasDrawNodes(ImDrawList* draw_list, const ImVec2 origin, const float zoom_level, const int font_size)
{
    const float node_padding = NODE_BORDER_OFFSET_BASE * zoom_level;

    for (; !m_parser.m_result_nodes_pq.empty(); m_parser.m_result_nodes_pq.pop()) {
        const auto& node = m_parser.m_result_nodes_pq.top();

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
        const auto node_x = static_cast<float>(node.position.x) * zoom_level;
        const auto node_y = static_cast<float>(node.position.y) * zoom_level;

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
        case TOPLEFT:
            break;
        case TOP:
            pivot_offset.x -= node_width / 2;
            break;
        case TOPRIGHT:
            pivot_offset.x -= node_width;
            break;
        case RIGHT:
            pivot_offset.x -= node_width;
            pivot_offset.y -= node_height / 2;
            break;
        case BOTTOMRIGHT:
            pivot_offset.x -= node_width;
            pivot_offset.y -= node_height;
            break;
        case BOTTOM:
            pivot_offset.x -= node_width / 2;
            pivot_offset.y -= node_height;
            break;
        case BOTTOMLEFT:
            pivot_offset.y -= node_height;
            break;
        case LEFT:
            pivot_offset.y -= node_height / 2;
            break;
        case CENTER:
            pivot_offset.x -= node_width / 2;
            pivot_offset.y -= node_height / 2;
            break;
        }

        // Calculate and store the AABR
        const ImVec2 aabr_top_left(node_x + parent_offset.x + pivot_offset.x,
                                   node_y + parent_offset.y + pivot_offset.y);
        const ImVec2 aabr_bottom_right(aabr_top_left.x + node_width,
                                       aabr_top_left.y + node_height);

        // ReSharper disable once CppUseStructuredBinding
        auto& canvas_node = m_canvas_nodes[node.id];
        canvas_node.top_left = aabr_top_left;
        canvas_node.bottom_right = aabr_bottom_right;
        canvas_node.center = ImVec2(aabr_top_left.x + node_width / 2,
                                    aabr_top_left.y + node_height / 2);

        // By adding origin (canvas position in window + scrolling) to AABR we get proper drawing coordinates
        const ImVec2 draw_top_left = origin + aabr_top_left;
        const ImVec2 draw_bottom_right = origin + aabr_bottom_right;

        // Do the actual drawing of the rectangle
        const auto z = DLUserChannelToRealChannel(node.z, true);
        draw_list->ChannelsSetCurrent(z);

        draw_list->AddRectFilled(draw_top_left,
                                 draw_bottom_right,
                                 GetColorFromTuple(node.color),
                                 0,
                                 0);

        constexpr auto COLOR_NODE_EDGE = IM_COL32(0, 0, 0, 255);
        draw_list->AddRect(draw_top_left, draw_bottom_right, COLOR_NODE_EDGE, 0, 0, zoom_level);

        m_exporter.AddRect(z, aabr_top_left.x, aabr_top_left.y, node_width, node_height, node.color);

        // Draw the label
        const auto label_left_x = draw_top_left.x + node_padding;
        const auto label_top_y = draw_top_left.y + node_padding;
        ImVec2 draw_label_position(label_left_x, label_top_y);

        // Custom label position?
        if (node.width > 0 || node.height > 0) {
            // Custom width/height => `label_pos` makes sense
            const ImVec2 draw_center = origin + canvas_node.center; // Helper variable
            switch (node.label_position) {
            case TOPLEFT:
                break;
            case TOP:
                draw_label_position = {
                    draw_center.x - label_size.x / 2,
                    label_top_y
                };
                break;
            case TOPRIGHT:
                draw_label_position = {
                    draw_bottom_right.x - label_size.x - node_padding,
                    label_top_y
                };
                break;
            case RIGHT:
                draw_label_position = {
                    draw_bottom_right.x - label_size.x - node_padding,
                    draw_center.y - label_size.y / 2
                };
                break;
            case BOTTOMRIGHT:
                draw_label_position = {
                    draw_bottom_right.x - label_size.x - node_padding,
                    draw_bottom_right.y - label_size.y - node_padding
                };
                break;
            case BOTTOM:
                draw_label_position = {
                    draw_center.x - label_size.x / 2,
                    draw_bottom_right.y - label_size.y - node_padding
                };
                break;
            case BOTTOMLEFT:
                draw_label_position = {
                    label_left_x,
                    draw_bottom_right.y - label_size.y - node_padding
                };
                break;
            case LEFT:
                draw_label_position = {
                    label_left_x,
                    draw_center.y - label_size.y / 2
                };
                break;
            case CENTER:
                draw_label_position = {
                    draw_center.x - label_size.x / 2,
                    draw_center.y - label_size.y / 2
                };
                break;
            }
        }

        draw_list->AddText(m_font_inconsolata_medium,
                           static_cast<float>(font_size),
                           draw_label_position,
                           COLOR_NODE_EDGE,
                           label_c_str);

        m_exporter.AddText(z, draw_label_position.x - origin.x, draw_label_position.y - origin.y, node.value);
    }
}
