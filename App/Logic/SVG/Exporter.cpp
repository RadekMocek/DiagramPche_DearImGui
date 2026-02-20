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

void Exporter::AddRect(const int z, const double x, const double y, const double width, const double height,
                       const std::tuple<unsigned char, unsigned char, unsigned char, unsigned char>& color)
{
    if (!m_is_enabled) return;

    m_draw_commands.push({
        z, PRIORITY_RECT,
        std::make_unique<svg::Rectangle>(svg::Point(x, y), width, height, svg::Fill(color), STROKE_BLACK)
    });

    if (x < m_boundaries_min_x) {
        m_boundaries_min_x = x;
    }
    if (y < m_boundaries_min_y) {
        m_boundaries_min_y = y;
    }
    if (x + width > m_boundaries_max_x) {
        m_boundaries_max_x = x + width;
    }
    if (y + height > m_boundaries_max_y) {
        m_boundaries_max_y = y + height;
    }
}

void Exporter::AddText(const int z, const double x, const double y, const std::string& value)
{
    if (!m_is_enabled) return;

    double y_offset = 0;
    const auto y_start = y - LINE_HEIGHT / 6; // Magic

    std::istringstream iss(value);
    std::string line;

    while (std::getline(iss, line)) {
        m_draw_commands.push(
            {
                z, PRIORITY_TEXT,
                std::make_unique<svg::Text>(svg::Point(x, y_start + y_offset), line, FILL_BLACK, FONT)
            });
        y_offset += LINE_HEIGHT;
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

    m_polyline_points.emplace_back(x, y);

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

void Exporter::FinishPolyLine(const int z, const int z2,
                              const std::tuple<unsigned char, unsigned char, unsigned char, unsigned char>& color)
{
    if (!m_is_enabled) return;

    const svg::Color svg_color(color);
    m_draw_commands.push(
        {
            z, z2,
            std::make_unique<svg::Polyline>(m_polyline_points, svg::Fill(std::get<3>(color)), svg::Stroke(1, svg_color))
        });
}

void Exporter::AddArrowTip(
    const int z, const int z2, const double p1_x, const double p1_y, const double p2_x, const double p2_y,
    const std::tuple<unsigned char, unsigned char, unsigned char, unsigned char>& color)
{
    if (!m_is_enabled) return;

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
