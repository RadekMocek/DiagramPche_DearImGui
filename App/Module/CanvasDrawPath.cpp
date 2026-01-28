#include "../App.hpp"
#include "../HelperFunction.hpp"

void App::ModuleCanvasDrawPaths(ImDrawList* draw_list, const ImVec2 origin, const float zoom_level)
{
    constexpr auto COLOR_PATH = IM_COL32(0, 0, 0, 255);

    for (const auto& path : m_parser.m_result_paths) {
        float prev_point_x = origin.x + static_cast<float>(path.start.x) * zoom_level;
        float prev_point_y = origin.y + static_cast<float>(path.start.y) * zoom_level;

        ImVec2 parent_offset(0, 0);
        if (!path.start.parent_id.empty()) {
            if (const auto it = m_canvas_nodes.find(path.start.parent_id); it != m_canvas_nodes.end()) {
                parent_offset = it->second.GetAnchor(path.start.parent_pivot);
            }
        }

        prev_point_x += parent_offset.x;
        prev_point_y += parent_offset.y;
    }

    /*
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
    /**/
}
