#include <algorithm>

#include "../App.hpp"
#include "../Helper/DrawLayer.hpp"
#include "../Helper/GUILayout.hpp"
#include "../Model/CanvasNode.hpp"

// ---  Canvas config  --- --- --- --- --- --- --- --- --- --- ---
constexpr ImVec2 SCROLLING_DEFAULT = {5.0f, 5.0f};
constexpr auto GRID_STEP_BASE = 100.0f;
constexpr auto COLOR_GRID_LINE = IM_COL32(200, 200, 200, 40);
constexpr auto COLOR_CANVAS_LIGHT = IM_COL32(240, 240, 240, 255);
constexpr auto CANVAS_FONT_SIZE_BASE = 18;
constexpr auto CANVAS_FONT_SIZE_STEP = 4;
constexpr auto CANVAS_FONT_SIZE_MIN = 6;
constexpr auto CANVAS_FONT_SIZE_MAX = 30;
constexpr auto CANVAS_SECONDARY_TOOLBAR_HEIGHT = 26.0f;
constexpr auto NODE_BORDER_OFFSET_BASE = 18.0f;
// --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

void App::ResetCanvasScrollingAndZoom()
{
    m_scrolling = SCROLLING_DEFAULT;
    m_canvas_font_size = CANVAS_FONT_SIZE_BASE;
    m_canvas_zoom_level = 1.0f;
}

void App::GUICanvas(const float height)
{
    // For getting the mouse position
    static ImGuiIO& io = ImGui::GetIO();
    // Dear ImGui sliders don't support step size, so we have to map 6,10,14,18,... to 1,2,3,4,... (Straightening tool for bending machine)
    static int zoom_level_slider_value = 3;

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
    const bool is_canvas_hovered = ImGui::IsItemHovered(); // Hovered (hot item)
    const bool is_canvas_active = ImGui::IsItemActive(); // Held

    // RMB drag to move the canvas ("scrolling")
    // Pan (we use a zero mouse threshold when there's no context menu)
    // You may decide to make that threshold dynamic based on whether the mouse is hovering something etc.
    if (constexpr float mouse_threshold_for_pan = -1.0f;
        is_canvas_active && ImGui::IsMouseDragging(ImGuiMouseButton_Right, mouse_threshold_for_pan)) {
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
    if (is_canvas_hovered && io.MouseWheel != 0) {
        const auto old_zoom = m_canvas_zoom_level;

        const int font_size_unclamped = m_canvas_font_size + io.MouseWheel * CANVAS_FONT_SIZE_STEP; // NOLINT(*-narrowing-conversions)
        ChangeCanvasFontSizeAndZoom(std::clamp(font_size_unclamped, CANVAS_FONT_SIZE_MIN, CANVAS_FONT_SIZE_MAX));
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

    draw_list->PushClipRect(canvas_top_left, canvas_bottom_right, true); // Draw only inside this rect

    if (m_style_do_force_light_canvas && m_style_current_color_theme != AppearanceTheme_Light) {
        // (No need to add light canvas background if we're using the light theme)
        draw_list->AddRectFilled(canvas_top_left, canvas_bottom_right, COLOR_CANVAS_LIGHT);
    }

    // == Draw grid ==
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
    GUICanvasDrawNodes(draw_list, origin, NODE_BORDER_OFFSET_BASE);
    // Default layer for paths is 5 (Model → Path.hpp → int z)
    GUICanvasDrawPaths(draw_list, origin);
    draw_list->ChannelsMerge();

    // .: User AABR interaction :.
    // .:=======================:.

    // == Clicking on nodes in canvas ==
    // If selected node is removed from the TOML source, unselect it
    if (m_is_canvas_node_selected && !m_parser.m_result_nodes.contains(m_selected_canvas_node_key)) {
        m_is_canvas_node_selected = false;
    }
    // Check for nodes under the cursor
    if (is_canvas_hovered) {
        static int hovered_node_z_mul;
        // We need to check if some node is hovered. If there are multiple nodes under the cursor,
        // we should choose the one with biggest `z_mul` (bigger z_mul == "closer" to the cursor).
        hovered_node_z_mul = -1;
        m_selected_or_hovered_canvas_node_key = std::nullopt;

        for (const auto& [key, value] : m_canvas_nodes) {
            if (value.z_mul > hovered_node_z_mul && value.IsPointInsideIncl(mouse_pos_in_canvas)) {
                //is_some_node_hovered = true;
                hovered_node_z_mul = value.z_mul;
                m_selected_or_hovered_canvas_node_key = key;
            }
        }
        // If there is some node under the cursor, then:
        // * LMB w/o modifiers selects the node so it can be modified via the toolbar
        // * Ctrl+LMB to jump to node's definition in the text editor
        if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
            // Clicked on node
            if (m_selected_or_hovered_canvas_node_key.has_value()) {
                // Ctrl+LMB
                if (io.KeyCtrl) {
                    if (m_do_use_alt_editor) {
                        const auto& hovered_node = m_canvas_nodes[m_selected_or_hovered_canvas_node_key.value()];
                        m_alt_editor.SetCursorPosition({hovered_node.def_line_num, 0});
                    }
                    // Vanilla textedit cannot move cursor, at least not without digging in imgui_internal.h, so show error instead
                    else {
                        ShowErrorModal("Vanilla text editor does not support jumping to node's definition.");
                    }
                }
                // LMB
                else {
                    m_is_canvas_node_selected = true;
                    m_selected_canvas_node_key = m_selected_or_hovered_canvas_node_key.value();
                }
            }
            // Clicked on empty space in canvas
            else {
                m_is_canvas_node_selected = false;
            }
        }
    }

    // == Drag n drop new node logic ==
    if (m_is_dragndropping_node) {
        //  For pointer position, we ignore scrolling here, we are checking if pointer is in part of the window
        // Do not draw the ghost node if in this position releasing LMB won't place it
        if (const auto mouse_pos_in_canvas_frame = io.MousePos - canvas_top_left;
            mouse_pos_in_canvas_frame >= ImVec2(0, 0) && mouse_pos_in_canvas_frame <= canvas_size) {
            // Prepare for the ghost node drawing
            const auto node_padding = NODE_BORDER_OFFSET_BASE * m_canvas_zoom_level;

            const auto ghost_label = std::format("node_{}", m_canvas_nodes.size());
            const auto ghost_label_c_str = ghost_label.c_str();

            const auto ghost_label_size = m_font_inconsolata_medium->
                CalcTextSizeA(static_cast<float>(m_canvas_font_size), FLT_MAX, -1.0f, ghost_label_c_str);

            const ImVec2 ghost_padding(ghost_label_size.x / 2 + node_padding,
                                       ghost_label_size.y / 2 + node_padding);
            // Draw the "ghost node"
            GUICanvasDrawGhostNode(draw_list, io.MousePos, node_padding, ghost_padding, ghost_label_c_str);
            // Check if LMB released inside the canvas
            if (!ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
                m_is_dragndropping_node = false;
                // Add new node to canvas (TOML values are zoom level independent so we divide by that)
                const auto node_x = static_cast<int>((mouse_pos_in_canvas.x - ghost_padding.x) / m_canvas_zoom_level);
                const auto node_y = static_cast<int>((mouse_pos_in_canvas.y - ghost_padding.y) / m_canvas_zoom_level);
                m_source += std::format("\n[node.{}]\ntype = \"{}\"\nxy = [{}, {}]\n",
                                        ghost_label, GetStringFromNodeType(m_dragndropping_node_type), node_x, node_y);
                OnMSourceChanged(true);
                // Make the new node selected
                m_is_canvas_node_selected = true;
                m_selected_canvas_node_key = ghost_label;
                if (m_do_use_alt_editor) {
                    m_alt_editor.SetCursorPosition({m_alt_editor.GetTotalLines() - 4, 0});
                }
            }
        }
        else {
            // Check if LMB released outside the canvas
            if (!ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
                m_is_dragndropping_node = false;
            }
        }
    }

    // Canvas drawing complete
    draw_list->PopClipRect();

    // .: Secondary canvas toolbar :.
    // .:==========================:.
    if (m_do_show_secondary_canvas_toolbar) {
        // Different background color for secondary toolbar
        const auto& cursor_screen_pos = ImGui::GetCursorScreenPos();
        draw_list->AddRectFilled(cursor_screen_pos,
                                 cursor_screen_pos + ImGui::GetContentRegionAvail(),
                                 m_style_color_secondary_toolbar);

        // == Add node buttons ==
        for (int i = 0; i < N_NTYPES; i++) {
            ImGui::Button(GetIconFromNodeType(static_cast<NodeType>(i)));

            if (!m_is_dragndropping_node && ImGui::IsItemActive()) {
                m_dragndropping_node_type = static_cast<NodeType>(i);
                m_is_dragndropping_node = true;
            }

            if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort | ImGuiHoveredFlags_NoSharedDelay)) {
                ImGui::SetTooltip("%s", std::format("Drag and drop me onto the canvas to add a '{}' node.",
                                                    GetStringFromNodeType(static_cast<NodeType>(i))).c_str());
            }

            SameLineWithDummy(NANO_SKIP);
        }

        // == Zoom level slider ==
        constexpr auto SLIDER_WIDTH = 200;
        ImGui::PushItemWidth(SLIDER_WIDTH);
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - SLIDER_WIDTH);
        // ReSharper disable once CppTooWideScopeInitStatement
        constexpr auto SLIDER_MIN = 0;
        constexpr auto SLIDER_MAX = (CANVAS_FONT_SIZE_MAX - CANVAS_FONT_SIZE_MIN) / CANVAS_FONT_SIZE_STEP;
        const auto slider_label = std::format("Zoom level: {:.2f}", m_canvas_zoom_level);
        if (ImGui::SliderInt("##ZoomLevel", &zoom_level_slider_value, SLIDER_MIN, SLIDER_MAX,
                             slider_label.c_str(), ImGuiSliderFlags_NoInput)) {
            ChangeCanvasFontSizeAndZoomFromSliderValue(zoom_level_slider_value);
        }
        ImGui::PopItemWidth();
    }

    ImGui::EndChild();
}

void App::ChangeCanvasFontSizeAndZoom(const int new_canvas_font_size)
{
    m_canvas_font_size = new_canvas_font_size;
    m_canvas_zoom_level = static_cast<float>(m_canvas_font_size) / static_cast<float>(CANVAS_FONT_SIZE_BASE);
}

void App::ChangeCanvasFontSizeAndZoomFromSliderValue(const int value)
{
    ChangeCanvasFontSizeAndZoom(CANVAS_FONT_SIZE_MIN + CANVAS_FONT_SIZE_STEP * value);
}
