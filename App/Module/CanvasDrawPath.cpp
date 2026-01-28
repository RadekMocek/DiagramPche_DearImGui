#include "../App.hpp"
#include "../HelperFunction.hpp"

void App::ModuleCanvasDrawPaths(ImDrawList* draw_list, const ImVec2 origin, const float zoom_level)
{
    constexpr auto COLOR_PATH = IM_COL32(0, 0, 0, 255);

    for (const auto& path : m_parser.m_result_paths) {
        // = START =
        ImVec2 start(static_cast<float>(path.start.x) * zoom_level, static_cast<float>(path.start.y) * zoom_level);

        if (!path.start.parent_id.empty()) {
            if (const auto it = m_canvas_nodes.find(path.start.parent_id); it != m_canvas_nodes.end()) {
                start = ImVec2Sum(start, it->second.GetExactPointFromPivot(path.start.parent_pivot));
            }
        }

        for (const auto& path_end : path.ends) {
            // = END =
            ImVec2 end(static_cast<float>(path_end.x) * zoom_level, static_cast<float>(path_end.y) * zoom_level);

            if (!path_end.parent_id.empty()) {
                if (const auto it = m_canvas_nodes.find(path_end.parent_id); it != m_canvas_nodes.end()) {
                    end = ImVec2Sum(end, it->second.GetExactPointFromPivot(path_end.parent_pivot));
                }
            }

            ImVec2 prev(start);

            for (const auto& path_point : path.path_points) {
                ImVec2 curr(static_cast<float>(path_point.x) * zoom_level,
                            static_cast<float>(path_point.y) * zoom_level);

                // X
                switch (path_point.x_type) {
                case UNKNOWN_PATHPOINTTYPE:
                    break;
                case REFERENCE:
                    if (const auto it = m_canvas_nodes.find(path_point.x_parent_id); it != m_canvas_nodes.end()) {
                        curr.x += it->second.GetExactPointFromPivot(path_point.x_parent_pivot).x;
                    }
                    break;
                case ABSOLUTE:
                    break;
                case START:
                    curr.x += start.x;
                    break;
                case END:
                    curr.x += end.x;
                    break;
                case PREVIOUS:
                    curr.x += prev.x;
                    break;
                }

                // Y
                switch (path_point.y_type) {
                case UNKNOWN_PATHPOINTTYPE:
                    break;
                case REFERENCE:
                    if (const auto it = m_canvas_nodes.find(path_point.y_parent_id); it != m_canvas_nodes.end()) {
                        curr.y += it->second.GetExactPointFromPivot(path_point.y_parent_pivot).y;
                    }
                    break;
                case ABSOLUTE:
                    break;
                case START:
                    curr.y += start.y;
                    break;
                case END:
                    curr.y += end.y;
                    break;
                case PREVIOUS:
                    curr.y += prev.y;
                    break;
                }

                //
                draw_list->AddLine(ImVec2Sum(prev, origin), ImVec2Sum(curr, origin), COLOR_PATH, zoom_level);

                prev = curr;
            }

            draw_list->AddLine(ImVec2Sum(prev, origin), ImVec2Sum(end, origin), COLOR_PATH, zoom_level);
        }
    }
}
