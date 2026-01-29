#include <algorithm>
#include <cmath>

#include "../App.hpp"
#include "../Config.hpp"
#include "../Model/CanvasNode.hpp"
#include "../HelperFunction.hpp"

void App::ModuleCanvas()
{
    static ImGuiIO& io = ImGui::GetIO(); // For getting the mouse position

    // .: Options and state :.
    // .:===================:.
    static ImVec2 scrolling(0.0f, 0.0f); // Scrolling means moving the canvas in this context
    static bool is_grid_enabled = true;

    constexpr auto COLOR_GRID_LINE = IM_COL32(200, 200, 200, 40);

    constexpr int FONT_SIZE_BASE = 18;
    constexpr int FONT_SIZE_STEP = 4;
    constexpr int FONT_SIZE_MIN = 6;
    constexpr int FONT_SIZE_MAX = 30;

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
    const ImVec2 mouse_pos_in_canvas(io.MousePos.x - origin.x, io.MousePos.y - origin.y);

    // Mousewheel to adjust the zoom level
    if (is_hovered) {
        const int font_size_unclamped = font_size + io.MouseWheel * FONT_SIZE_STEP;
        font_size = std::clamp(font_size_unclamped, FONT_SIZE_MIN, FONT_SIZE_MAX);
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

        for (float x = fmodf(scrolling.x, GRID_STEP); x < canvas_size.x; x += GRID_STEP) { // NOLINT(*-flp30-c)
            draw_list->AddLine(ImVec2(canvas_top_left.x + x, canvas_top_left.y),
                               ImVec2(canvas_top_left.x + x, canvas_bottom_right.y),
                               COLOR_GRID_LINE);
        }

        for (float y = fmodf(scrolling.y, GRID_STEP); y < canvas_size.y; y += GRID_STEP) { // NOLINT(*-flp30-c)
            draw_list->AddLine(ImVec2(canvas_top_left.x, canvas_top_left.y + y),
                               ImVec2(canvas_bottom_right.x, canvas_top_left.y + y),
                               COLOR_GRID_LINE);
        }
    }

    // = Draw the diagram =

    // AABR = axis aligned bounding rectangle :)
    // This map is used to store some additional info about nodes and also to keep track about which nodes were already drawn.
    // One thing we need to store is node's AABR. Relative nodes, which are drawn later, can then use it to determine their position. Paths also need AABR info.
    m_canvas_nodes.clear();

    // 9 draw layers which can be set by user in TOML with values: 0, 1, 2, 3, 4, 5, 6, 7, 8
    draw_list->ChannelsSplit(N_DRAW_LIST_CHANNELS);
    // Default draw layer for nodes is 4 (see Model → Node.hpp → int z)
    ModuleCanvasDrawNodes(draw_list, origin, zoom_level, font_size);
    // Default layer for paths is 5
    draw_list->ChannelsSetCurrent(DRAW_LIST_CHANNEL_DEFAULT_PATH);
    ModuleCanvasDrawPaths(draw_list, origin, zoom_level);
    draw_list->ChannelsMerge();

    // .: User AABR interaction :.
    //  .:======================:.

    // Show tooltip with Node ID on hover
    static std::string tooltip;
    tooltip.clear();
    for (const auto& [key, value] : m_canvas_nodes) {
        if (value.IsPointInsideIncl(mouse_pos_in_canvas)) {
            tooltip += ", " + key;
        }
    }
    if (!tooltip.empty()) {
        ImGui::SetTooltip("%s", tooltip.substr(2).c_str());
    }

    ImGui::EndChild();
}
