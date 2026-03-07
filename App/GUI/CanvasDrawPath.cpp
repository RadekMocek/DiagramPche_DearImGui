#include "../App.hpp"
#include "../Helper/Color.hpp"
#include "../Helper/Draw.hpp"
#include "../Helper/DrawLayer.hpp"

void App::GUICanvasDrawPaths(ImDrawList* draw_list, const ImVec2 origin)
{
    const auto font_size_f = static_cast<float>(m_canvas_font_size);

    // For SVG export → `Exporter.DrawCommand.same_z_priority`. Arrow tips should be on the same exact "SVG layer" as their path.
    // If there were multiple colliding paths, then without this, tips could be sorted differently than their corresponsing paths
    // (and that could be seen if colliding paths have different colors).
    int path_number = 0;

    for (const auto& path : m_parser.m_result_paths) {
        // Setup
        path_number++;
        const auto z = DLUserChannelToRealChannel(path.z, false);
        draw_list->ChannelsSetCurrent(z);

        // Get the "simple" values from path
        const auto color = GetImU32FromColorTuple(path.color);

        // Prepare the start point
        ImVec2 start(static_cast<float>(path.start.x) * m_canvas_zoom_level,
                     static_cast<float>(path.start.y) * m_canvas_zoom_level);
        bool do_start_shift = false;
        if (!path.start.parent_id.empty()) {
            if (const auto it = m_canvas_nodes.find(path.start.parent_id); it != m_canvas_nodes.end()) {
                // Move start point so it's relative to parent's pivot
                start += it->second.GetExactPointFromPivot(path.start.parent_pivot);
                // Path shift makes sense only when the start/end point is relative to some node
                if (path.shift_start != 0) {
                    do_start_shift = true;
                }
            }
        }

        // Start is now "originated" (takes canvas origin into account)
        start += origin;

        // One path can have multiple ends defined by the user => one [[path]] can define multiple result_paths.
        // Every result_path will be defined as vector of points Vec<Pos2> (start point, maybe some Pathpoints, end point).
        // Vector of these vectors will be given to the draw command.
        std::vector<std::vector<ImVec2>> result_paths;
        // (This could be rewritten w/o the need to store all of this in `result_paths` because each iteration is independent)

        // Each inner vector starts with the start point; or, if shift != 0 && start point is relative, with OG start point followed by a shifted start point
        if (!do_start_shift) {
            result_paths = std::vector(path.ends.size(), std::vector({start}));
        }
        else {
            const auto shifted_start = start + path.GetShiftVector(path.start.parent_pivot, m_canvas_zoom_level, true);
            result_paths = std::vector(path.ends.size(), std::vector({start, shifted_start}));
            start = shifted_start; // Do this so Pathpoints relative to start are relative to this
        }

        // Foreach end point
        auto index = 0;
        for (const auto& path_end : path.ends) {
            auto& result_pathpoints = result_paths[index++];

            // Ready the current end point
            ImVec2 end(static_cast<float>(path_end.x) * m_canvas_zoom_level,
                       static_cast<float>(path_end.y) * m_canvas_zoom_level);
            bool do_end_shift = false;
            if (!path_end.parent_id.empty()) {
                if (const auto it = m_canvas_nodes.find(path_end.parent_id); it != m_canvas_nodes.end()) {
                    end += it->second.GetExactPointFromPivot(path_end.parent_pivot);
                    if (path.shift_end != 0) {
                        do_end_shift = true;
                    }
                }
            }

            // End is now "originated"
            end += origin;

            // `shifted_end` is the end point, that all the Pathpoints relate to.
            // If there is no a shift, it is just the original end.
            // If there is a shift, we apply it; we still remember the original end and in this case it will be the last point added to current collection.
            const auto shifted_end = (!do_end_shift)
                                         ? end
                                         : end + path.GetShiftVector(path_end.parent_pivot, m_canvas_zoom_level, false);

            // Pathpoints (defined as a collection [[path]].points) are points between start and end.
            // They are not mandatory: if no Pathpoints are specified, then path is just a single line from start to end.
            // If there are some, we iterate them and add them to the result collection.
            auto prev(start); // This is for the "prev" to work (Pathpoint relative to previous Pathpoint)

            // ReSharper disable once CppUseStructuredBinding
            for (const auto& pathpoint : path.pathpoints) {
                // Currently processed Pathpoint
                ImVec2 curr(static_cast<float>(pathpoint.x) * m_canvas_zoom_level,
                            static_cast<float>(pathpoint.y) * m_canvas_zoom_level);
                // Apply the Pathpoint type for both coordinates
                // X
                switch (pathpoint.x_type) {
                // Absolute coordinates are, by definition, not "originated"
                case PPTYPE_ABSOLUTE:
                    curr.x += origin.x;
                    break;
                // AABRs in `m_canvas_nodes` are stored "zoomed and absolute", so they take zoom_level into account, but not origin.
                // So we have to add origin here, later code depends on "originated" Pathpoint.
                case PPTYPE_REFERENCE:
                    if (const auto it = m_canvas_nodes.find(pathpoint.x_parent_id); it != m_canvas_nodes.end()) {
                        curr.x += it->second.GetExactPointFromPivot(pathpoint.x_parent_pivot).x + origin.x;
                    }
                    break;
                // Start, End, and Prev are "originated", so we mustn't add origin here
                case PPTYPE_START:
                    curr.x += start.x;
                    break;
                case PPTYPE_END:
                    curr.x += shifted_end.x;
                    break;
                case PPTYPE_PREVIOUS:
                    curr.x += prev.x;
                    break;
                }
                // Same for Y
                switch (pathpoint.y_type) {
                case PPTYPE_ABSOLUTE:
                    curr.y += origin.y;
                    break;
                case PPTYPE_REFERENCE:
                    if (const auto it = m_canvas_nodes.find(pathpoint.y_parent_id); it != m_canvas_nodes.end()) {
                        curr.y += it->second.GetExactPointFromPivot(pathpoint.y_parent_pivot).y + origin.y;
                    }
                    break;
                case PPTYPE_START:
                    curr.y += start.y;
                    break;
                case PPTYPE_END:
                    curr.y += shifted_end.y;
                    break;
                case PPTYPE_PREVIOUS:
                    curr.y += prev.y;
                    break;
                }

                // Pathpoint is ready now
                result_pathpoints.push_back(curr);

                // Ready for the next iteration
                prev = curr;
            }

            // After the Pathpoints are ready add the endpoint(s)
            result_pathpoints.push_back(shifted_end);
            if (do_end_shift) {
                result_pathpoints.push_back(end);
            }

            // DRAW current path
            if (result_pathpoints.size() >= 2) {
                // Line from all the points
                draw_list->AddPolyline(result_pathpoints.data(),
                                       result_pathpoints.size(),
                                       color,
                                       0,
                                       m_canvas_zoom_level);
                // Start
                if (path.do_start_arrow) {
                    DrawArrowTip(draw_list,
                                 result_pathpoints[1],
                                 result_pathpoints[0],
                                 m_canvas_zoom_level,
                                 color);
                }
                // End arrow
                if (path.do_end_arrow) {
                    DrawArrowTip(draw_list,
                                 result_pathpoints[result_pathpoints.size() - 2],
                                 result_pathpoints[result_pathpoints.size() - 1],
                                 m_canvas_zoom_level,
                                 color);
                }

                // Path label
                if (!path.label_value.empty()) {
                    const auto& label_c_str = path.label_value.c_str();
                    const auto label_point_curr_idx = path.label_point % result_pathpoints.size();
                    const auto label_point_next_idx = (path.label_point + 1) % result_pathpoints.size();
                    const auto label_shift = path.label_shift;

                    const auto label_point_curr = result_pathpoints[label_point_curr_idx];
                    const auto label_point_next = result_pathpoints[label_point_next_idx];
                    const auto direction = ImVec2Normalized(label_point_next - label_point_curr);

                    const auto label_position = label_point_curr + direction * label_shift;
                    draw_list->AddText(m_font_inconsolata_medium,
                                       font_size_f,
                                       label_position,
                                       COLOR_BLACK,
                                       label_c_str);
                }
            }
        }
        // [[path]] to SVG
        for (const auto& result_path : result_paths) {
            m_exporter.StartPolyLine();
            for (const auto& point : result_path) {
                m_exporter.AddPointToPolyLine(point.x, point.y);
            }
            m_exporter.FinishPolyLine(z, path_number, path.color);
            if (result_path.size() >= 2) {
                if (path.do_start_arrow) {
                    m_exporter.AddArrowTip(z, path_number,
                                           result_path[1].x,
                                           result_path[1].y,
                                           result_path[0].x,
                                           result_path[0].y,
                                           path.color);
                }
                if (path.do_end_arrow) {
                    m_exporter.AddArrowTip(z, path_number,
                                           result_path[result_path.size() - 2].x,
                                           result_path[result_path.size() - 2].y,
                                           result_path[result_path.size() - 1].x,
                                           result_path[result_path.size() - 1].y,
                                           path.color);
                }
            }
        }
    }
}
