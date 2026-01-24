#include <algorithm>
#include <cmath>
#include <ranges>

#include "../App.hpp"
#include "../HelperFunction.hpp"
#include "../Model/CanvasNodeStruct.hpp"

void App::ModuleCanvas()
{
    static ImGuiIO& io = ImGui::GetIO(); // For getting the mouse position

    // .: Options and state :.
    // .:===================:.
    static ImVec2 scrolling(0.0f, 0.0f); // Scrolling means moving the canvas in this context
    static bool is_grid_enabled = true;
    static float zoom_level = 1.0f;

    constexpr auto ZOOM_STEP = 0.2f;
    constexpr auto ZOOM_LEVEL_MIN = 0.1f;
    constexpr auto ZOOM_LEVEL_MAX = 2.0f;

    constexpr auto FONT_SIZE_BASE = 18;

    constexpr auto COLOR_GRID_LINE = IM_COL32(200, 200, 200, 40);
    constexpr auto COLOR_NODE = IM_COL32(0, 0, 0, 255);

    // Padding of rectangle border around text in canvas
    constexpr float NODE_BORDER_OFFSET_BASE = 18.0f;
    const float node_border_offset = NODE_BORDER_OFFSET_BASE * zoom_level;

    // .: Prepare ground for the canvas :.
    // .:===============================:.
    const auto content_region_available = ImGui::GetContentRegionAvail();
    const ImVec2 canvas_size(content_region_available.x, content_region_available.y - BOTTOM_BAR_HEIGHT);

    // Create a parent for our canvas (with zero padding)
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::BeginChild("CanvasParent",
                      canvas_size,
                      ImGuiChildFlags_Borders,
                      ImGuiWindowFlags_None);
    ImGui::PopStyleVar();

    // Determine canvas size
    const auto canvas_top_left = ImGui::GetCursorScreenPos(); // ImDrawList API uses screen coordinates!
    const auto canvas_bottom_right = ImVec2(canvas_top_left.x + canvas_size.x, canvas_top_left.y + canvas_size.y);

    // Canvas origin position (window absolute)
    const ImVec2 origin(canvas_top_left.x + scrolling.x, canvas_top_left.y + scrolling.y);

    // .: User interaction :.
    // .:==================:.
    // Using InvisibleButton() will advance the layout cursor and allows us to use IsItemHovered()/IsItemActive()
    ImGui::InvisibleButton("Canvas", canvas_size, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);
    const bool is_hovered = ImGui::IsItemHovered(); // Hovered (hot item)
    const bool is_active = ImGui::IsItemActive(); // Held
    //const ImVec2 mouse_pos_in_canvas(io.MousePos.x - origin.x, io.MousePos.y - origin.y);

    // Mousewheel to adjust the zoom level
    if (is_hovered) {
        const float new_zoom_level = zoom_level + io.MouseWheel * ZOOM_STEP;
        zoom_level = std::clamp(new_zoom_level, ZOOM_LEVEL_MIN, ZOOM_LEVEL_MAX);
        //IMGUI_DEBUG_LOG("%f\n", zoom_level);
        //todo node_border_offset is using this above so there is a 1 frame delay
    }

    // RMB drag to move the canvas ("scrolling")
    // Pan (we use a zero mouse threshold when there's no context menu)
    // You may decide to make that threshold dynamic based on whether the mouse is hovering something etc.
    constexpr float mouse_threshold_for_pan = -1.0f;
    if (is_active && ImGui::IsMouseDragging(ImGuiMouseButton_Right, mouse_threshold_for_pan)) {
        scrolling.x += io.MouseDelta.x;
        scrolling.y += io.MouseDelta.y;
    }

    // .: Draw on canvas :.
    // .:================:.
    ImDrawList* draw_list = ImGui::GetWindowDrawList(); // Enables us to draw primitives

    // Draw grid + all lines in the canvas
    draw_list->PushClipRect(canvas_top_left, canvas_bottom_right, true);
    if (is_grid_enabled) {
        constexpr float GRID_STEP_BASE = 100.0f;
        const float GRID_STEP = GRID_STEP_BASE * zoom_level;

        for (float x = fmodf(scrolling.x, GRID_STEP); x < canvas_size.x; x += GRID_STEP) {
            draw_list->AddLine(ImVec2(canvas_top_left.x + x, canvas_top_left.y),
                               ImVec2(canvas_top_left.x + x, canvas_bottom_right.y),
                               COLOR_GRID_LINE);
        }

        for (float y = fmodf(scrolling.y, GRID_STEP); y < canvas_size.y; y += GRID_STEP) {
            draw_list->AddLine(ImVec2(canvas_top_left.x, canvas_top_left.y + y),
                               ImVec2(canvas_bottom_right.x, canvas_top_left.y + y),
                               COLOR_GRID_LINE);
        }
    }

    // Draw nodes on the canvas
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

    // Draw paths on the canvas
    for (const auto& path : m_parser.m_result_paths) {
        bool is_first_iter = true;

        float prev_point_x{};
        float prev_point_y{};

        for (const auto& point : path.points) {
            float point_x = origin.x + static_cast<float>(point.x) * zoom_level;
            float point_y = origin.y + static_cast<float>(point.y) * zoom_level;

            if (is_first_iter) {
                is_first_iter = false;
            }
            else {
                draw_list->AddLine({prev_point_x, prev_point_y}, {point_x, point_y}, COLOR_NODE, zoom_level);
            }

            prev_point_x = point_x;
            prev_point_y = point_y;
        }
    }

    ImGui::EndChild();
}
