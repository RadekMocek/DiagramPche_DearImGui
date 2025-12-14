#include <algorithm>
#include <cmath>

#include "../App.hpp"
#include "../Helper/HelperFunctions.hpp"

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

    //
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
    for (const auto& node : m_parser.m_result_nodes) {
        // Draw text
        const auto label_c_str = node.value.c_str();
        const auto node_x = static_cast<float>(node.x) * zoom_level;
        const auto node_y = static_cast<float>(node.y) * zoom_level;

        auto label_origin = ImVec2(origin.x + node_x, origin.y + node_y);
        draw_list->AddText(m_font_inconsolata_medium,
                           FONT_SIZE_BASE * zoom_level,
                           label_origin,
                           COLOR_NODE,
                           label_c_str);

        // Draw rectangle around the text
        const auto label_size = m_font_inconsolata_medium->CalcTextSizeA(FONT_SIZE_BASE * zoom_level,
                                                                         FLT_MAX,
                                                                         -1.0f,
                                                                         label_c_str);

        const auto rect_top_left = ImVec2Translation(label_origin, -node_border_offset);
        const auto rect_bottom_right = ImVec2Translation(ImVec2Sum(label_origin, label_size), node_border_offset);
        draw_list->AddRect(rect_top_left, rect_bottom_right, COLOR_NODE, 0, 0, zoom_level);
    }

    ImGui::EndChild();
}
