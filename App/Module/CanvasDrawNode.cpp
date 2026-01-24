#include <ranges>

#include "../App.hpp"
#include "../Model/CanvasNodeStruct.hpp"
#include "../HelperFunction.hpp"

void App::ModuleCanvasDrawNodes(ImDrawList* draw_list, const ImVec2 origin, const float zoom_level)
{
    constexpr auto COLOR_NODE = IM_COL32(0, 0, 0, 255);
    constexpr auto FONT_SIZE_BASE = 18;
    // Padding of rectangle border around text in canvas
    constexpr float NODE_BORDER_OFFSET_BASE = 18.0f;
    const float node_border_offset = NODE_BORDER_OFFSET_BASE * zoom_level;

    std::unordered_map<std::string, CanvasNode> canvas_nodes{};
    int current_draw_batch_number = -1;
    while (m_parser.m_result_nodes_map.size() != canvas_nodes.size()) {
        current_draw_batch_number++;
        for (const auto& node : m_parser.m_result_nodes_map | std::views::values) {
            if (node.draw_batch_number == current_draw_batch_number) {
                // We need to know the AABR size for the pivot
                const ImVec2 pad(node_border_offset, node_border_offset);

                const auto label_c_str = node.value.c_str();

                const auto label_size = m_font_inconsolata_medium->CalcTextSizeA(
                    FONT_SIZE_BASE * zoom_level, FLT_MAX, -1.0f, label_c_str
                );

                const auto node_x = static_cast<float>(node.x) * zoom_level;
                const auto node_y = static_cast<float>(node.y) * zoom_level;

                ImVec2 base_offset(0, 0);
                if (!node.base_id.empty()) {
                    if (const auto it = canvas_nodes.find(node.base_id); it != canvas_nodes.end()) {
                        const auto& parent = it->second;
                        switch (node.base_pivot) {
                        case UNKNOWN:
                            break; // todo fatal
                        case TOPLEFT:
                            base_offset = parent.top_left;
                            break;
                        case TOP:
                            base_offset = {parent.center.x, parent.top_left.y};
                            break;
                        case TOPRIGHT:
                            base_offset = {parent.bottom_right.x, parent.top_left.y};
                            break;
                        case RIGHT:
                            base_offset = {parent.bottom_right.x, parent.center.y};
                            break;
                        case BOTTOMRIGHT:
                            base_offset = parent.bottom_right;
                            break;
                        case BOTTOM:
                            base_offset = {parent.center.x, parent.bottom_right.y};
                            break;
                        case BOTTOMLEFT:
                            base_offset = {parent.top_left.x, parent.bottom_right.y};
                            break;
                        case LEFT:
                            base_offset = {parent.top_left.x, parent.center.y};
                            break;
                        case CENTER:
                            base_offset = parent.center;
                            break;
                        }
                    } // todo else fatal
                }

                const auto node_width = label_size.x + 2 * node_border_offset;
                const auto node_height = label_size.y + 2 * node_border_offset;

                ImVec2 pivot_offset(0, 0);

                switch (node.pivot) {
                case UNKNOWN:
                    break; // todo fatal
                case TOPLEFT:
                    break; // Nothing to do
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

                const ImVec2 aabr_top_left(node_x + base_offset.x + pivot_offset.x,
                                           node_y + base_offset.y + pivot_offset.y);
                const ImVec2 aabr_bottom_right(aabr_top_left.x + node_width,
                                               aabr_top_left.y + node_height);

                auto& canvas_node = canvas_nodes[node.id];
                canvas_node.top_left = aabr_top_left;
                canvas_node.bottom_right = aabr_bottom_right;
                canvas_node.center = ImVec2(aabr_top_left.x + node_width / 2,
                                            aabr_top_left.y + node_height / 2);

                const ImVec2 draw_top_left = ImVec2Sum(origin, aabr_top_left);
                const ImVec2 draw_top_right = ImVec2Sum(origin, aabr_bottom_right);
                draw_list->AddRect(draw_top_left, draw_top_right, COLOR_NODE, 0, 0, zoom_level);

                draw_list->AddText(m_font_inconsolata_medium,
                                   FONT_SIZE_BASE * zoom_level,
                                   ImVec2Sum(draw_top_left, pad),
                                   COLOR_NODE,
                                   label_c_str);
            }
        }
    }
}
