#include "Exporter.hpp"
#include "../../Config.hpp"

void Exporter::Start(const std::string& path)
{
    m_is_enabled = true;
    m_path = path;
    m_draw_commands = std::priority_queue<DrawCommand>();
    m_boundaries_min_x = std::numeric_limits<double>::max();
    m_boundaries_min_y = std::numeric_limits<double>::max();
    m_boundaries_max_x = std::numeric_limits<double>::min();
    m_boundaries_max_y = std::numeric_limits<double>::min();
}

bool Exporter::Save()
{
    constexpr svg::Layout::Origin ORIGIN = svg::Layout::TopLeft;

    if (!m_is_enabled) return false;
    m_is_enabled = false;

    const svg::Dimensions dimensions(m_boundaries_max_x - m_boundaries_min_x + 2 * SVG_PADDING,
                                     m_boundaries_max_y - m_boundaries_min_y + 2 * SVG_PADDING);

    const auto layout = svg::Layout(dimensions, ORIGIN);

    svg::Document document(m_path, layout);

    for (; !m_draw_commands.empty(); m_draw_commands.pop()) {
        const auto& [z1, z2, shape] = m_draw_commands.top();
        shape->offset(svg::Point(SVG_PADDING - m_boundaries_min_x, SVG_PADDING - m_boundaries_min_y));
        document << *shape;
    }

    return document.save();
}

void Exporter::AddRect(const int z, const double tl_x, const double tl_y, const double width, const double height,
                       const ColorTuple& color)
{
    if (!m_is_enabled) return;

    // Draw command
    m_draw_commands.push({
        z, PRIORITY_SHAPE,
        std::make_unique<svg::Rectangle>(svg::Point(tl_x, tl_y), width, height, svg::Fill(color), STROKE_BLACK)
    });

    // Update boundaries
    if (tl_x < m_boundaries_min_x) {
        m_boundaries_min_x = tl_x;
    }
    if (tl_y < m_boundaries_min_y) {
        m_boundaries_min_y = tl_y;
    }
    if (tl_x + width > m_boundaries_max_x) {
        m_boundaries_max_x = tl_x + width;
    }
    if (tl_y + height > m_boundaries_max_y) {
        m_boundaries_max_y = tl_y + height;
    }
}

void Exporter::AddEllipse(const int z, const double c_x, const double c_y, const double width, const double height,
                          const ColorTuple& color)
{
    if (!m_is_enabled) return;

    // Draw command
    m_draw_commands.push({
        z, PRIORITY_SHAPE,
        std::make_unique<svg::Elipse>(svg::Point(c_x, c_y), width, height, svg::Fill(color), STROKE_BLACK)
    });

    // Update boundaries
    const auto radius_x = width / 2.0;
    const auto left = c_x - radius_x;
    const auto right = c_x + radius_x;
    const auto radius_y = height / 2.0;
    const auto top = c_y - radius_y;
    const auto bottom = c_y + radius_y;
    if (left < m_boundaries_min_x) {
        m_boundaries_min_x = left;
    }
    if (top < m_boundaries_min_y) {
        m_boundaries_min_y = top;
    }
    if (right > m_boundaries_max_x) {
        m_boundaries_max_x = right;
    }
    if (bottom > m_boundaries_max_y) {
        m_boundaries_max_y = bottom;
    }
}

void Exporter::AddDiamond(const int z, const double c_x, const double c_y,
                          const double t_y, const double r_x, const double b_y, const double l_x,
                          const ColorTuple& color)
{
    if (!m_is_enabled) return;

    // Draw command
    const svg::Point top(c_x, t_y);
    const svg::Point right(r_x, c_y);
    const svg::Point bottom(c_x, b_y);
    const svg::Point left(l_x, c_y);

    svg::Polygon polygon{svg::Fill(color), STROKE_BLACK};
    polygon << top << right << bottom << left;
    m_draw_commands.push({z, PRIORITY_SHAPE, std::make_unique<svg::Polygon>(polygon)});

    // Update boundaries
    if (l_x < m_boundaries_min_x) {
        m_boundaries_min_x = l_x;
    }
    if (t_y < m_boundaries_min_y) {
        m_boundaries_min_y = t_y;
    }
    if (r_x > m_boundaries_max_x) {
        m_boundaries_max_x = r_x;
    }
    if (b_y > m_boundaries_max_y) {
        m_boundaries_max_y = b_y;
    }
}

void Exporter::AddText(const int z, const double tl_x, const double tl_y, const std::string& value)
{
    if (!m_is_enabled) return;

    // Draw command
    double y_offset = 0;
    const auto y_start = tl_y - LINE_HEIGHT / 6; // Magic

    std::istringstream iss(value);
    std::string line;

    size_t max_line_length = 0;

    while (std::getline(iss, line)) {
        m_draw_commands.push(
            {
                z, PRIORITY_TEXT,
                std::make_unique<svg::Text>(svg::Point(tl_x, y_start + y_offset), line, FILL_BLACK, FONT)
            });
        y_offset += LINE_HEIGHT;

        if (line.size() > max_line_length) {
            max_line_length = line.size();
        }
    }

    // Update boundaries
    if (tl_x < m_boundaries_min_x) {
        m_boundaries_min_x = tl_x;
    }
    if (y_start < m_boundaries_min_y) {
        m_boundaries_min_y = y_start;
    }
    if (const auto text_max_x = tl_x + CHAR_WIDTH * static_cast<double>(max_line_length);
        text_max_x > m_boundaries_max_x) {
        m_boundaries_max_x = text_max_x;
    }
    if (y_start + y_offset > m_boundaries_max_y) {
        m_boundaries_max_y = y_start + y_offset;
    }
}

void Exporter::StartPolyLine()
{
    if (!m_is_enabled) return;
    m_polyline_points.clear();
}

void Exporter::AddPointToPolyLine(double x, double y)
{
    if (!m_is_enabled) return;

    // Add point for future draw command
    m_polyline_points.emplace_back(x, y);

    // Update boundaries
    if (x < m_boundaries_min_x) {
        m_boundaries_min_x = x;
    }
    if (x > m_boundaries_max_x) {
        m_boundaries_max_x = x;
    }
    if (y < m_boundaries_min_y) {
        m_boundaries_min_y = y;
    }
    if (y > m_boundaries_max_y) {
        m_boundaries_max_y = y;
    }
}

void Exporter::FinishPolyLine(const int z, const int z2, const ColorTuple& color)
{
    if (!m_is_enabled) return;

    // Draw command
    const svg::Color svg_color(color);
    m_draw_commands.push(
        {
            z, z2,
            std::make_unique<svg::Polyline>(m_polyline_points, svg::Fill(std::get<3>(color)), svg::Stroke(1, svg_color))
        });
}

void Exporter::AddArrowTip(
    const int z, const int z2, const double p1_x, const double p1_y, const double p2_x, const double p2_y,
    const ColorTuple& color)
{
    if (!m_is_enabled) return;

    // Draw command
    const svg::Point p1(p1_x, p1_y);
    const svg::Point p2(p2_x, p2_y);

    const auto p2_to_p1 = svg::Point::normalized(p1 - p2);
    const auto point_slightly_before_p2 = p2 + p2_to_p1 * TIP_ARROW_LENGTH;
    const auto p2_orthogonal_addition = svg::Point::orthogonalized(p2_to_p1) * TIP_ARROW_SPAN;

    svg::Polygon polygon{svg::Fill(color), svg::Stroke()};
    polygon
        << p2
        << point_slightly_before_p2 - p2_orthogonal_addition
        << point_slightly_before_p2 + p2_orthogonal_addition;
    m_draw_commands.push({z, z2, std::make_unique<svg::Polygon>(polygon)});

    // Currently, I don't feel there is a need to update boundaries after adding an arrow tip
}

// --- --- --- --- --- --- --- ---
/*
void Exporter::DebugPrint() const
{
    std::cout << "m_boundaries_max_x: " << m_boundaries_max_x << '\n';
    std::cout << "m_boundaries_min_x: " << m_boundaries_min_x << '\n';
    std::cout << "m_boundaries_max_y: " << m_boundaries_max_y << '\n';
    std::cout << "m_boundaries_min_y: " << m_boundaries_min_y << '\n';

    std::cout << "width = " << m_boundaries_max_x << "-" << m_boundaries_min_x
        << " = " << m_boundaries_max_x - m_boundaries_min_x << '\n';

    std::cout << "height = " << m_boundaries_max_y << "-" << m_boundaries_min_y
        << " = " << m_boundaries_max_y - m_boundaries_min_y << '\n';

    std::cout << '\n';
}
*/
