#include <algorithm>
#include <cmath>

#include "../App.hpp"
#include "../HelperFunction.hpp"

void App::ModuleCanvas()
{
    static ImGuiIO& io = ImGui::GetIO(); // For getting the mouse position

    // .: Options and state :.
    // .:===================:.
    static ImVec2 scrolling(0.0f, 0.0f); // Scrolling means moving the canvas in this context
    static bool is_grid_enabled = true;

    constexpr auto COLOR_GRID_LINE = IM_COL32(200, 200, 200, 40);
    constexpr auto COLOR_PATH = IM_COL32(0, 0, 0, 255);

    constexpr int FONT_SIZE_BASE = 18;
    constexpr int FONT_SIZE_STEP = 4;
    constexpr int FONT_SIZE_MIN = 8;
    constexpr int FONT_SIZE_MAX = 28;

    static int font_size = FONT_SIZE_BASE;
    static float zoom_level = 1.0f;

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
        const int new_font_size = font_size + io.MouseWheel * FONT_SIZE_STEP;
        font_size = std::clamp(new_font_size, FONT_SIZE_MIN, FONT_SIZE_MAX);
        zoom_level = font_size / static_cast<float>(FONT_SIZE_BASE);
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
    ModuleCanvasDrawNodes(draw_list, origin, zoom_level, font_size);

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
                draw_list->AddLine({prev_point_x, prev_point_y}, {point_x, point_y}, COLOR_PATH, zoom_level);
            }

            prev_point_x = point_x;
            prev_point_y = point_y;
        }
    }

    ImGui::EndChild();
}
