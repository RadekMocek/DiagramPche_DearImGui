#include <algorithm>

#include "../App.hpp"
#include "../Helper/DrawLayer.hpp"
#include "../Model/CanvasNode.hpp"
#include "../Config.hpp"

void App::GUICanvas(const float height)
{
    static ImGuiIO& io = ImGui::GetIO(); // For getting the mouse position

    // .: Prepare ground for the canvas :.
    // .:===============================:.
    const ImVec2 canvas_size(ImGui::GetContentRegionAvail().x, height);

    // Create a parent for our canvas (with zero padding)
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::BeginChild("CanvasParent",
                      canvas_size,
                      ImGuiChildFlags_Borders,
                      ImGuiWindowFlags_None);
    ImGui::PopStyleVar();

    // Determine canvas position (window absolute)
    const auto canvas_top_left = ImGui::GetCursorScreenPos(); // ImDrawList API uses screen coordinates!
    const auto canvas_bottom_right = canvas_top_left + canvas_size;

    // .: User interaction :.
    // .:==================:.
    // Using InvisibleButton() will advance the layout cursor and allows us to use IsItemHovered()/IsItemActive()
    ImGui::InvisibleButton("Canvas", canvas_size, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);
    const bool is_hovered = ImGui::IsItemHovered(); // Hovered (hot item)
    const bool is_active = ImGui::IsItemActive(); // Held

    // RMB drag to move the canvas ("scrolling")
    // Pan (we use a zero mouse threshold when there's no context menu)
    // You may decide to make that threshold dynamic based on whether the mouse is hovering something etc.
    if (constexpr float mouse_threshold_for_pan = -1.0f;
        is_active && ImGui::IsMouseDragging(ImGuiMouseButton_Right, mouse_threshold_for_pan)) {
        m_scrolling.x += io.MouseDelta.x;
        m_scrolling.y += io.MouseDelta.y;
        // Change cursor shape
        glfwSetCursor(m_window, m_cursor_crosshair);
        m_is_glfw_cursor_used = true;
    }
    else if (m_is_glfw_cursor_used) {
        glfwSetCursor(m_window, nullptr);
        m_is_glfw_cursor_used = false;
    }

    // Calculate canvas "origin" (position + scrolling), used for drawing
    auto origin = canvas_top_left + m_scrolling;
    auto mouse_pos_in_canvas = io.MousePos - origin;

    // Mousewheel to adjust the zoom level
    if (is_hovered && io.MouseWheel != 0) {
        const auto old_zoom = m_canvas_zoom_level;

        const int font_size_unclamped = m_canvas_font_size + io.MouseWheel * CANVAS_FONT_SIZE_STEP;
        m_canvas_font_size = std::clamp(font_size_unclamped, CANVAS_FONT_SIZE_MIN, CANVAS_FONT_SIZE_MAX);
        m_canvas_zoom_level = m_canvas_font_size / static_cast<float>(CANVAS_FONT_SIZE_BASE);

        // Zoom anchor under mouse
        if (const auto new_zoom = m_canvas_zoom_level; old_zoom != new_zoom) {
            const auto ratio = new_zoom / old_zoom;
            m_scrolling += mouse_pos_in_canvas * (1.0f - ratio);
            origin = canvas_top_left + m_scrolling;
            mouse_pos_in_canvas = io.MousePos - origin;
        }
    }

    // If we are creating a SVG this frame, we reset zoom_level here so we don't have have to "revert it" in the SVG.
    // This is the place to do it because we already handled the user interaction this frame (RMB scroll and MW zoom).
    if (m_exporter.IsEnabled()) {
        ResetCanvasScrollingAndZoom();
    }

    // .: Draw on canvas :.
    // .:================:.
    ImDrawList* draw_list = ImGui::GetWindowDrawList(); // Enables us to draw primitives

    // == Draw grid ==
    draw_list->PushClipRect(canvas_top_left, canvas_bottom_right, true);
    if (m_do_show_grid) {
        const float GRID_STEP = GRID_STEP_BASE * m_canvas_zoom_level;

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

    // == Draw diagram ==

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
    GUICanvasDrawNodes(draw_list, origin, m_canvas_zoom_level, m_canvas_font_size);
    // Default layer for paths is 5 (Model → Path.hpp → int z)
    GUICanvasDrawPaths(draw_list, origin, m_canvas_zoom_level);
    draw_list->ChannelsMerge();

    // .: User AABR interaction :.
    // .:=======================:.
    // Show tooltip with Node ID on hover
    if (is_hovered) {
        static bool is_some_node_hovered;
        static std::string hovered_node_key;
        static int hovered_node_z_mul;

        is_some_node_hovered = false;
        hovered_node_z_mul = -1;

        for (const auto& [key, value] : m_canvas_nodes) {
            if (value.z_mul > hovered_node_z_mul && value.IsPointInsideIncl(mouse_pos_in_canvas)) {
                is_some_node_hovered = true;
                hovered_node_key = key;
                hovered_node_z_mul = value.z_mul;
            }
        }
        if (is_some_node_hovered) {
            // Tooltip with selected node id
            //ImGui::SetTooltip("%s", hovered_node_key.c_str());

            // Left click on a hovered node
            if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                const auto& hovered_node = m_canvas_nodes[hovered_node_key];
                // Ctrl + Leftclick to show node's definition in the text editor
                if (io.KeyCtrl) {
                    if (m_do_use_alt_editor) {
                        m_alt_editor.SetCursorPosition({hovered_node.def_line_num, 0});
                    }
                    else {
                        ShowErrorModal("Vanilla text editor does not support jumping to node's definition.");
                    }
                }
                // Leftclick w/o modifiers to select the node
                else {
                    m_selected_or_hovered_canvas_node_key = hovered_node_key;
                    m_is_canvas_node_selected = true;
                }
            }

            if (!m_is_canvas_node_selected) {
                m_selected_or_hovered_canvas_node_key = hovered_node_key;
            }
        }
        else {
            // No node hovered
            if (m_is_canvas_node_selected) {
                if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                    // Left click to an empty space when some node is selected
                    m_selected_or_hovered_canvas_node_key = std::nullopt;
                    m_is_canvas_node_selected = false;
                }
            }
            else {
                m_selected_or_hovered_canvas_node_key = std::nullopt;
            }
        }
    }

    ImGui::EndChild();
}
