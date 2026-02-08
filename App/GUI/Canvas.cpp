#include <algorithm>
#include <cmath>

#include "../App.hpp"
#include "../Helper/DrawLayer.hpp"
#include "../Model/CanvasNode.hpp"
#include "../Config.hpp"

void App::GUICanvas()
{
    static ImGuiIO& io = ImGui::GetIO(); // For getting the mouse position

    // .: State :.
    // .:=======:.
    static int font_size = CANVAS_FONT_SIZE_BASE;
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
    const ImVec2 origin(canvas_top_left.x + m_scrolling.x, canvas_top_left.y + m_scrolling.y);

    // .: User interaction :.
    // .:==================:.
    // Using InvisibleButton() will advance the layout cursor and allows us to use IsItemHovered()/IsItemActive()
    ImGui::InvisibleButton("Canvas", canvas_size, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);
    const bool is_hovered = ImGui::IsItemHovered(); // Hovered (hot item)
    const bool is_active = ImGui::IsItemActive(); // Held
    const ImVec2 mouse_pos_in_canvas(io.MousePos.x - origin.x, io.MousePos.y - origin.y);

    // Mousewheel to adjust the zoom level
    if (is_hovered) {
        const int font_size_unclamped = font_size + io.MouseWheel * CANVAS_FONT_SIZE_STEP;
        font_size = std::clamp(font_size_unclamped, CANVAS_FONT_SIZE_MIN, CANVAS_FONT_SIZE_MAX);
        zoom_level = font_size / static_cast<float>(CANVAS_FONT_SIZE_BASE);
    }

    // RMB drag to move the canvas ("scrolling")
    // Pan (we use a zero mouse threshold when there's no context menu)
    // You may decide to make that threshold dynamic based on whether the mouse is hovering something etc.
    if (constexpr float mouse_threshold_for_pan = -1.0f;
        is_active && ImGui::IsMouseDragging(ImGuiMouseButton_Right, mouse_threshold_for_pan)) {
        m_scrolling.x += io.MouseDelta.x;
        m_scrolling.y += io.MouseDelta.y;
    }

    // .: Draw on canvas :.
    // .:================:.
    ImDrawList* draw_list = ImGui::GetWindowDrawList(); // Enables us to draw primitives

    // Draw grid
    draw_list->PushClipRect(canvas_top_left, canvas_bottom_right, true);
    if (m_do_show_grid) {
        const float GRID_STEP = GRID_STEP_BASE * zoom_level;

        for (float x = fmodf(m_scrolling.x, GRID_STEP); x < canvas_size.x; x += GRID_STEP) { // NOLINT(*-flp30-c)
            draw_list->AddLine(ImVec2(canvas_top_left.x + x, canvas_top_left.y),
                               ImVec2(canvas_top_left.x + x, canvas_bottom_right.y),
                               COLOR_GRID_LINE);
        }

        for (float y = fmodf(m_scrolling.y, GRID_STEP); y < canvas_size.y; y += GRID_STEP) { // NOLINT(*-flp30-c)
            draw_list->AddLine(ImVec2(canvas_top_left.x, canvas_top_left.y + y),
                               ImVec2(canvas_bottom_right.x, canvas_top_left.y + y),
                               COLOR_GRID_LINE);
        }
    }

    // = Draw the diagram =

    // AABR = axis aligned bounding rectangle :)
    // This map is used to store some additional info about nodes and also to keep track about which nodes were already drawn.
    // One thing we need to store is node's AABR. Relative nodes, which are drawn later, can then use it to determine their position. Paths also need AABR info.
    // Stored AABRs take zoom_level into account, but not scrolling.
    m_canvas_nodes.clear();

    // 10 draw layers which can be set by user in TOML with values: 0, 1, 2, 3, 4, 5, 6, 7, 8, 9
    // But number of real layers is actually 20 to ensure that path is always above node when they have the same 'z' value
    // (We use 2*z for nodes and 2*z+1 for paths)
    draw_list->ChannelsSplit(N_DL_REAL_CHANNELS);
    // Default draw layer for nodes is 4 (Model → Node.hpp → int z)
    GUICanvasDrawNodes(draw_list, origin, zoom_level, font_size);
    // Default layer for paths is 5 (Model → Path.hpp → int z)
    GUICanvasDrawPaths(draw_list, origin, zoom_level);
    draw_list->ChannelsMerge();

    // .: User AABR interaction :.
    // .:=======================:.

    // Show tooltip with Node ID on hover
    if (is_hovered) {
        static std::string tooltip;
        tooltip.clear();
        for (const auto& [key, value] : m_canvas_nodes) {
            if (value.IsPointInsideIncl(mouse_pos_in_canvas)) {
                // Remove newlines
                auto key_copy = key;
                std::ranges::replace(key_copy, '\n', ' ');
                tooltip += ", " + key_copy;
            }
        }
        if (!tooltip.empty()) {
            ImGui::SetTooltip("%s", tooltip.substr(2).c_str());
        }
    }

    ImGui::EndChild();
}
