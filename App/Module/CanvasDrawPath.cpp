#include "../App.hpp"
#include "../HelperFunction.hpp"

void App::ModuleCanvasDrawPaths(ImDrawList* draw_list, const ImVec2 origin, const float zoom_level)
{
    for (const auto& path : m_parser.m_result_paths) {
        const auto color = GetColorFromTuple(path.color);
        const auto shift = path.shift;

        // Ready the start point
        // AABRs in `m_canvas_nodes` are stored (zoomed and absolute), so they take zoom_level into account, but not origin.
        // That's why we multiply by zoom_level right now, but origin is added after all the calculations, just before the the drawing.
        ImVec2 start(static_cast<float>(path.start.x) * zoom_level, static_cast<float>(path.start.y) * zoom_level);
        bool is_start_arrow_satisfied = !path.do_start_arrow;
        // Is the start point realative to some node?
        if (!path.start.parent_id.empty()) {
            if (const auto it = m_canvas_nodes.find(path.start.parent_id); it != m_canvas_nodes.end()) {
                // Move so it's relative to parent's pivot
                start += it->second.GetExactPointFromPivot(path.start.parent_pivot);
                // If shift is set, draw line from OG start to shifted start, and update the start variable
                if (shift != 0) {
                    const auto shifted_start = start + path.GetShiftDirection(path.start.parent_pivot, zoom_level);

                    const auto p1 = start + origin;
                    const auto p2 = shifted_start + origin;
                    draw_list->AddLine(p1, p2, color, zoom_level);

                    if (!is_start_arrow_satisfied) {
                        DrawArrowTip(draw_list, p2, p1, zoom_level, color);
                        is_start_arrow_satisfied = true;
                    }

                    start = shifted_start;
                }
            }
        }

        // Foreach end point (there can be multiple end points for effective definition of multiple paths):
        for (const auto& path_end : path.ends) {
            // Ready the current end point
            ImVec2 end(static_cast<float>(path_end.x) * zoom_level, static_cast<float>(path_end.y) * zoom_level);
            bool is_end_arrow_satisfied = !path.do_end_arrow;
            // Is the end point realative to some node?
            if (!path_end.parent_id.empty()) {
                if (const auto it = m_canvas_nodes.find(path_end.parent_id); it != m_canvas_nodes.end()) {
                    end += it->second.GetExactPointFromPivot(path_end.parent_pivot);
                }
                // If shift is set, draw line from OG end to shifted end, and update the end variable
                if (shift != 0) {
                    const auto shifted_end = end + path.GetShiftDirection(path_end.parent_pivot, zoom_level);

                    const auto p1 = shifted_end + origin;
                    const auto p2 = end + origin;
                    draw_list->AddLine(p1, p2, color, zoom_level);

                    if (!is_end_arrow_satisfied) {
                        DrawArrowTip(draw_list, p1, p2, zoom_level, color);
                        is_end_arrow_satisfied = true;
                    }

                    end = shifted_end;
                }
            }

            // Pathspoints (defined as a collection [[path]].points) are points between start and end.
            // They are not mandatory: if no Pathpoints are specified, then path is just a single line from start to end.
            // If there are some, we iterate them and always draw line from previous point to current point.
            ImVec2 prev(start); // Firts line will be from start to first Pathpoint

            for (const auto& path_point : path.path_points) {
                // Currently processed Pathpoint
                ImVec2 curr(static_cast<float>(path_point.x) * zoom_level,
                            static_cast<float>(path_point.y) * zoom_level);

                // For both coordinates apply the Pathpoint type
                switch (path_point.x_type) {
                default: // UNKNOWN_PATHPOINTTYPE || ABSOLUTE
                    break;
                case REFERENCE:
                    if (const auto it = m_canvas_nodes.find(path_point.x_parent_id); it != m_canvas_nodes.end()) {
                        curr.x += it->second.GetExactPointFromPivot(path_point.x_parent_pivot).x;
                    }
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
                switch (path_point.y_type) {
                default: // UNKNOWN_PATHPOINTTYPE || ABSOLUTE
                    break;
                case REFERENCE:
                    if (const auto it = m_canvas_nodes.find(path_point.y_parent_id); it != m_canvas_nodes.end()) {
                        curr.y += it->second.GetExactPointFromPivot(path_point.y_parent_pivot).y;
                    }
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

                // Draw the line and set new previous point for next iteration
                const auto p1 = prev + origin;
                const auto p2 = curr + origin;
                draw_list->AddLine(p1, p2, color, zoom_level);

                if (!is_start_arrow_satisfied) {
                    DrawArrowTip(draw_list, p2, p1, zoom_level, color);
                    is_start_arrow_satisfied = true;
                }

                // Set for next iteration
                prev = curr;
            }



            // Last line from last Pathpoint to end
            const auto p1 = prev + origin;
            const auto p2 = end + origin;
            draw_list->AddLine(p1, p2, color, zoom_level);

            if (!is_end_arrow_satisfied) {
                DrawArrowTip(draw_list, p1, p2, zoom_level, color);
                is_end_arrow_satisfied = true;
            }

            if (!is_start_arrow_satisfied) {
                // This happens when there are no Pathpoints between start and end
                DrawArrowTip(draw_list, p2, p1, zoom_level, color);
                is_start_arrow_satisfied = true;
            }
        }
    }
}
