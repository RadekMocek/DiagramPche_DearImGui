#include <ranges>

#include "../App.hpp"
#include "../Config.hpp"
#include "../Model/CanvasNodeStruct.hpp"
#include "../HelperFunction.hpp"

void App::ModuleCanvasDrawNodes(ImDrawList* draw_list, const ImVec2 origin, const float zoom_level, const int font_size)
{
    constexpr float NODE_BORDER_OFFSET_BASE = 18.0f;
    const float node_padding = NODE_BORDER_OFFSET_BASE * zoom_level;

    // AABR = axis aligned bounding rectangle :)

    // This map is used to store some additional info about nodes and also to keep track about which nodes were already drawn.
    // One thing we need to store is node's AABR so relative nodes (which are drawn later) can use it to determine their position.
    std::unordered_map<std::string, CanvasNode> canvas_nodes{};
    // We will just iterate `m_result_nodes_map` multiple times until every node was drawn; we increment `current_draw_batch_number` with each iteration.
    // If we have a node with an accordant batch number and it wasn't drawn yet, we draw it.
    // Alternative approach would be to sort the collction first by nodes' batch number, but sorting map by value probably requires to convert it
    // to some other collection (priority queue?), which means std::moving all the node structs or doing some pointer bussiness. This works for now...
    int current_draw_batch_number = -1;
    while (m_parser.m_result_nodes_map.size() != canvas_nodes.size()) {
        current_draw_batch_number++; // 0, 1, 2, ...
        for (const auto& node : m_parser.m_result_nodes_map | std::views::values) {
            if (node.draw_batch_number == current_draw_batch_number) {
                // Dear ImGui text functions take `const char*`
                const auto label_c_str = node.value.c_str();

                // This gives us the size of label if we draw it; it's used for implicit (AAB)Rectangle size
                const auto label_size = m_font_inconsolata_medium->
                    CalcTextSizeA(font_size, FLT_MAX, -1.0f, label_c_str);

                // This is from the line `xy=[0,0]`
                const auto node_x = static_cast<float>(node.x) * zoom_level;
                const auto node_y = static_cast<float>(node.y) * zoom_level;

                // Move node according to its `base`, if user have set some; this is where we use stored AABR from `canvas_nodes`
                ImVec2 base_offset(0, 0);
                if (!node.base_id.empty()) {
                    if (const auto it = canvas_nodes.find(node.base_id); it != canvas_nodes.end()) {
                        const auto& parent = it->second;
                        switch (node.base_pivot) {
                        case UNKNOWN:
                            // Undefined, do nothing
                            break;
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
                    }
                    //else printerr("Node '"<<node.id<<"' is referencing '"<<node.base_id<<"', which wasn't drawn yet!");
                }

                const auto node_width = (node.width > 0) ? node.width * zoom_level : label_size.x + 2 * node_padding;
                const auto node_height = (node.height > 0) ? node.height * zoom_level : label_size.y + 2 * node_padding;

                // Move node according to its `pivot`, if user have set some
                ImVec2 pivot_offset(0, 0);

                switch (node.pivot) {
                case UNKNOWN:
                // Undefined, do nothing
                //break; // Fallthrough
                case TOPLEFT:
                    // Nothing to do
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

                // Calculate and store AABR
                const ImVec2 aabr_top_left(node_x + base_offset.x + pivot_offset.x,
                                           node_y + base_offset.y + pivot_offset.y);
                const ImVec2 aabr_bottom_right(aabr_top_left.x + node_width,
                                               aabr_top_left.y + node_height);

                auto& canvas_node = canvas_nodes[node.id];
                canvas_node.top_left = aabr_top_left;
                canvas_node.bottom_right = aabr_bottom_right;
                canvas_node.center = ImVec2(aabr_top_left.x + node_width / 2,
                                            aabr_top_left.y + node_height / 2);

                // By adding origin (canvas position in window + scrolling) to AABR we get proper drawing coordinates
                const ImVec2 draw_top_left = ImVec2Sum(origin, aabr_top_left);
                const ImVec2 draw_bottom_right = ImVec2Sum(origin, aabr_bottom_right);

                // Do the actual drawing of the rectangle
                draw_list->ChannelsSetCurrent(node.z + Z_DEPTH_ABS_MAX);

                constexpr auto COLOR_NODE = IM_COL32(0, 0, 0, 255);

                draw_list->AddRectFilled(draw_top_left,
                                         draw_bottom_right,
                                         IM_COL32(node.color_r, node.color_g, node.color_b, node.color_a),
                                         0,
                                         0);

                draw_list->AddRect(draw_top_left, draw_bottom_right, COLOR_NODE, 0, 0, zoom_level);

                // Draw label
                const auto label_left_x = draw_top_left.x + node_padding;
                const auto label_top_y = draw_top_left.y + node_padding;
                ImVec2 draw_label_position(label_left_x, label_top_y);

                if (node.width > 0 || node.height > 0) {
                    // Custom width/height => `text_pos` makes sense
                    // Helper vars:
                    const ImVec2 draw_center = ImVec2Sum(origin, canvas_node.center);
                    switch (node.label_position) {
                    case UNKNOWN:
                    // Undefined, do nothing
                    //break; // Fallthrough
                    case TOPLEFT:
                        // Nothing to do
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
                                   font_size,
                                   draw_label_position,
                                   COLOR_NODE,
                                   label_c_str);
            }
        }
    }
}
