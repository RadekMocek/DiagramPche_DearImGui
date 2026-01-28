#include "../App.hpp"
#include "../HelperFunction.hpp"

void App::ModuleCanvasDrawPaths(ImDrawList* draw_list, const ImVec2 origin, const float zoom_level)
{
    ImVec2 parent_offset{};

    constexpr auto COLOR_PATH = IM_COL32(0, 0, 0, 255);

    for (const auto& path : m_parser.m_result_paths) {
        // = START =
        float start_x = origin.x + static_cast<float>(path.start.x) * zoom_level;
        float start_y = origin.y + static_cast<float>(path.start.y) * zoom_level;

        parent_offset = {0, 0};
        if (!path.start.parent_id.empty()) {
            if (const auto it = m_canvas_nodes.find(path.start.parent_id); it != m_canvas_nodes.end()) {
                parent_offset = it->second.GetExactPointFromPivot(path.start.parent_pivot);
            }
        }

        start_x += parent_offset.x;
        start_y += parent_offset.y;

        // = END =
        float end_x = origin.x + static_cast<float>(path.end.x) * zoom_level;
        float end_y = origin.y + static_cast<float>(path.end.y) * zoom_level;

        parent_offset = {0, 0};
        if (!path.end.parent_id.empty()) {
            if (const auto it = m_canvas_nodes.find(path.end.parent_id); it != m_canvas_nodes.end()) {
                parent_offset = it->second.GetExactPointFromPivot(path.end.parent_pivot);
            }
        }

        end_x += parent_offset.x;
        end_y += parent_offset.y;

        //
        float prev_point_x = start_x;
        float prev_point_y = start_y;

        for (const auto& path_point : path.path_points) {
            // X
            auto x = origin.x + static_cast<float>(path_point.x) * zoom_level;

            switch (path_point.x_type) {
            case UNKNOWN_PATHPOINTTYPE:
                break;
            case REFERENCE:
                if (const auto it = m_canvas_nodes.find(path_point.x_parent_id); it != m_canvas_nodes.end()) {
                    x += it->second.GetExactPointFromPivot(path_point.x_parent_pivot).x;
                }
                break;
            case ABSOLUTE:
                break;
            case START:
                x += start_x;
                break;
            case END:
                x += end_x;
                break;
            case PREVIOUS:
                x += prev_point_x;
                break;
            }


            // Y
            auto y = origin.y + static_cast<float>(path_point.y) * zoom_level;

            switch (path_point.y_type) {
            case UNKNOWN_PATHPOINTTYPE:
                break;
            case REFERENCE:
                if (const auto it = m_canvas_nodes.find(path_point.y_parent_id); it != m_canvas_nodes.end()) {
                    y += it->second.GetExactPointFromPivot(path_point.y_parent_pivot).y;
                }
                break;
            case ABSOLUTE:
                break;
            case START:
                y += start_y;
                break;
            case END:
                y += end_y;
                break;
            case PREVIOUS:
                y += prev_point_y;
                break;
            }


            //
            draw_list->AddLine({prev_point_x, prev_point_y}, {x, y}, COLOR_PATH, zoom_level);

            prev_point_x = x;
            prev_point_y = y;
        }
    }
}
