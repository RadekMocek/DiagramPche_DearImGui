#include "Exporter.hpp"
#include "../../Config.hpp"

#define ReportD(x) //std::cout << "SVGExporter> " << x << '\n'
#define ReportF(x) std::cout << "SVGExporter> " << x << '\n'

void Exporter::Start()
{
    m_is_enabled = true;
    m_draw_commands = std::priority_queue<DrawCommand>();
    m_boundaries_min_x = 0;
    m_boundaries_min_y = 0;
    m_boundaries_max_x = 0;
    m_boundaries_max_y = 0;
}

void Exporter::Save()
{
    constexpr svg::Layout::Origin ORIGIN = svg::Layout::TopLeft;

    if (!m_is_enabled) return;
    m_is_enabled = false;

    const svg::Dimensions dimensions(m_boundaries_max_x - m_boundaries_min_x + SVG_PADDING,
                                     m_boundaries_max_y - m_boundaries_min_y + SVG_PADDING);
    const auto layout = svg::Layout(dimensions, ORIGIN);
    svg::Document document("test.svg", layout);

    for (; !m_draw_commands.empty(); m_draw_commands.pop()) {
        const auto& [z1, z2, shape] = m_draw_commands.top();
        shape->offset(svg::Point(SVG_PADDING / 2, SVG_PADDING / 2));
        document << *shape;
    }

    if (document.save()) {
        ReportF("File saved successfully");
    }
    else {
        ReportF("Failed to save the file");
    }
}

void Exporter::AddRect(const int z, const double x, const double y, const double width, const double height,
                       const std::tuple<unsigned char, unsigned char, unsigned char, unsigned char>& color)
{
    if (!m_is_enabled) return;
    ReportD("Adding rect: " << x << " " << y << " " << width << " " << height << " ...");
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
    ReportD("Adding text: '" << value << "'");

    //TODO magic numbers
    const auto font = svg::Font(17, "Inconsolata");
    constexpr auto line_height_ratio = 1.1;
    double y_offset = 0;
    const auto line_height = font.getSize() * line_height_ratio;
    const auto y_start = y - font.getSize() / 6;

    std::istringstream iss(value);
    std::string line;

    while (std::getline(iss, line)) {
        m_draw_commands.push(
            {
                z, PRIORITY_TEXT,
                std::make_unique<svg::Text>(svg::Point(x, y_start + y_offset), line, FILL_BLACK, font)
            });
        y_offset += line_height;
    }
}

void Exporter::StartPolyLine()
{
    if (!m_is_enabled) return;
    ReportD("== Starting PolyLine ==");
    m_polyline_points.clear();
}

void Exporter::AddPointToPolyLine(double x, double y)
{
    if (!m_is_enabled) return;
    ReportD("Adding point [" << x << ", " << y << "] to PolyLine");
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
    ReportD("== PolyLine done ==");
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
    ReportD("(Adding arrow tip)");

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
