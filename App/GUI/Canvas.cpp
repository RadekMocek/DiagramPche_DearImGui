#include <algorithm>

#include "../App.hpp"
#include "../Helper/DrawLayer.hpp"
#include "../Helper/Operator.hpp"
#include "../Model/CanvasNode.hpp"
#include "../Config.hpp"

void App::GUICanvas(const float height)
{
    static ImGuiIO& io = ImGui::GetIO(); // For getting the mouse position
    static int zoom_level_slider_value = 3; // Rovnák na ohýbák

    // .: Prepare ground for the canvas :.
    // .:===============================:.
    ImVec2 canvas_size(ImGui::GetContentRegionAvail().x, height);

    // Create a parent for our canvas (with zero padding)
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::BeginChild("CanvasParent",
                      canvas_size,
                      ImGuiChildFlags_Borders,
                      ImGuiWindowFlags_None);
    ImGui::PopStyleVar();

    // Do this after `BeginChild` so the border is not affected
    if (m_do_show_secondary_canvas_toolbar) {
        canvas_size.y -= CANVAS_SECONDARY_TOOLBAR_HEIGHT;
    }

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

        zoom_level_slider_value = (m_canvas_font_size - CANVAS_FONT_SIZE_MIN) / CANVAS_FONT_SIZE_STEP;

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
    draw_list->PushClipRect(canvas_top_left, canvas_bottom_right, true); // Draw only inside this rect
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
    const auto font_size_f = static_cast<float>(m_canvas_font_size);
    GUICanvasDrawNodes(draw_list, origin, m_canvas_zoom_level, font_size_f);
    // Default layer for paths is 5 (Model → Path.hpp → int z)
    GUICanvasDrawPaths(draw_list, origin, m_canvas_zoom_level);
    draw_list->ChannelsMerge();

    // .: User AABR interaction :.
    // .:=======================:.

    // == Drag n drop new node logic ==
    if (m_is_dragndropping_node) {
        constexpr ImVec2 VEC_ZERO(0, 0);
        constexpr auto COLOR_GHOST_EDGE = IM_COL32(0, 0, 0, 128);
        constexpr auto COLOR_GHOST_FILL = IM_COL32(255, 255, 255, 128);
        const auto node_padding = NODE_BORDER_OFFSET_BASE * m_canvas_zoom_level;

        // Draw the "ghost node"
        const auto ghost_label = std::format("node_{}", m_canvas_nodes.size());
        const auto ghost_label_c_str = ghost_label.c_str();
        const auto ghost_label_size = m_font_inconsolata_medium->
            CalcTextSizeA(font_size_f, FLT_MAX, -1.0f, ghost_label_c_str);
        const ImVec2 ghost_padding(ghost_label_size.x / 2 + node_padding,
                                   ghost_label_size.y / 2 + node_padding);
        const auto& mouse_pos = io.MousePos;
        const auto ghost_top_left = mouse_pos - ghost_padding;
        const auto ghost_bottom_right = mouse_pos + ghost_padding;
        draw_list->AddText(m_font_inconsolata_medium,
                           font_size_f,
                           ghost_top_left + ImVec2(node_padding, node_padding),
                           COLOR_GHOST_EDGE,
                           ghost_label_c_str);
        draw_list->AddRectFilled(ghost_top_left, ghost_bottom_right, COLOR_GHOST_FILL, 0, 0);
        draw_list->AddRect(ghost_top_left, ghost_bottom_right, COLOR_GHOST_EDGE, 0, 0, m_canvas_zoom_level);

        // Check if LMB released inside canvas
        if (!ImGui::IsMouseDown(ImGuiMouseButton_Left)
            && mouse_pos_in_canvas >= VEC_ZERO
            && mouse_pos_in_canvas <= canvas_size) {
            // Add new node to canvas
            const auto node_x = static_cast<int>(mouse_pos_in_canvas.x - ghost_padding.x);
            const auto node_y = static_cast<int>(mouse_pos_in_canvas.y - ghost_padding.y);
            m_source += std::format("\n[node.{}]\nxy = [{}, {}]\n", ghost_label, node_x, node_y);
            OnMSourceChanged();
        }
    }

    // Canvas drawing complete
    draw_list->PopClipRect();

    // == Clicking on nodes in canvas ==
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

    // .: Secondary canvas toolbar :.
    // .:==========================:.
    if (m_do_show_secondary_canvas_toolbar) {
        // Different background color for secondary toolbar
        const auto& cursor_screen_pos = ImGui::GetCursorScreenPos();
        draw_list->AddRectFilled(cursor_screen_pos,
                                 cursor_screen_pos + ImGui::GetContentRegionAvail(),
                                 IM_COL32(219, 219, 219, 255));

        // == Add node button ==
        ImGui::Button("Add node");
        // - "drag n drop functionality", not using Dear ImGui dragndrop capabilities here, this is more convenient in this situation
        m_is_dragndropping_node = ImGui::IsItemActive();
        // - tooltip
        if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort | ImGuiHoveredFlags_NoSharedDelay)) {
            ImGui::SetTooltip("Drag and drop me onto the canvas.");
        }

        // == Zoom level slider ==
        ImGui::SameLine();
        constexpr auto SLIDER_WIDTH = 200;
        ImGui::PushItemWidth(SLIDER_WIDTH);
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - SLIDER_WIDTH);
        // ReSharper disable once CppTooWideScopeInitStatement
        constexpr auto SLIDER_MIN = 0;
        constexpr auto SLIDER_MAX = (CANVAS_FONT_SIZE_MAX - CANVAS_FONT_SIZE_MIN) / CANVAS_FONT_SIZE_STEP;
        const auto slider_label = std::format("Zoom level: {:.2f}", m_canvas_zoom_level);
        if (ImGui::SliderInt("##ZoomLevel", &zoom_level_slider_value, SLIDER_MIN, SLIDER_MAX,
                             slider_label.c_str(), ImGuiSliderFlags_NoInput)) {
            m_canvas_font_size = CANVAS_FONT_SIZE_MIN + CANVAS_FONT_SIZE_STEP * zoom_level_slider_value;
            m_canvas_zoom_level = m_canvas_font_size / static_cast<float>(CANVAS_FONT_SIZE_BASE);
        }
        ImGui::PopItemWidth();
    }

    ImGui::EndChild();
}
