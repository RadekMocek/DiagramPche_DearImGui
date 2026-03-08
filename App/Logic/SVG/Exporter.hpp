#pragma once

#include <queue>

#include "../../../Dependency/svg.hpp"

using ColorTuple = std::tuple<unsigned char, unsigned char, unsigned char, unsigned char>;

class Exporter
{
private:
    static constexpr auto SVG_DOCUMENT_PADDING = 25.0f;
    static constexpr auto SVG_PRIORITY_SHAPE = 0;
    static constexpr auto SVG_PRIORITY_TEXT = 1;

    const svg::Fill SVG_FILL_BLACK = svg::Fill(svg::Color::Black, 1.0);
    const svg::Font SVG_FONT = svg::Font(18, "Inconsolata");
    const svg::Size SVG_CHAR_BB = svg::Text(svg::Point(0, 0), "A", SVG_FILL_BLACK, SVG_FONT).getBoundingBox().size;
    const double SVG_LINE_HEIGHT = SVG_CHAR_BB.height;
    const double SVG_CHAR_WIDTH = SVG_CHAR_BB.width;

    struct DrawCommand
    {
        int z;
        // This decides the priority if the `z`s are equal:
        // - Nodes: text should be above shape
        // - Paths: arrow tips should be on exact same layer as their path
        int same_z_priority;

        std::unique_ptr<svg::Shape> shape;

        bool operator<(const DrawCommand& other) const
        {
            if (z != other.z) {
                return z > other.z;
            }
            return same_z_priority > other.same_z_priority;
        }
    };

    bool m_is_enabled{};
    std::string m_path;
    std::priority_queue<DrawCommand> m_draw_commands;

    double m_boundaries_min_x{};
    double m_boundaries_min_y{};
    double m_boundaries_max_x{};
    double m_boundaries_max_y{};

    std::vector<svg::Point> m_polyline_points{};

    //void DebugPrint() const;

public:
    [[nodiscard]] bool IsEnabled() const { return m_is_enabled; }

    //
    void Start(const std::string& path);
    bool Save();

    // Shapes
    void AddRect(int z, double tl_x, double tl_y, double width, double height,
                 const ColorTuple& color, const ColorTuple& color_border, int additional_priority = 0);

    void AddEllipse(int z, double c_x, double c_y, double width, double height,
                    const ColorTuple& color, const ColorTuple& color_border);

    void AddDiamond(int z, double c_x, double c_y, double t_y, double r_x, double b_y, double l_x,
                    const ColorTuple& color, const ColorTuple& color_border);

    // Text
    void AddText(int z, double tl_x, double tl_y, const std::string& value, int additional_priority = 0);

    // Line
    void StartPolyLine();
    void AddPointToPolyLine(double x, double y);
    void FinishPolyLine(int z, int z2, const ColorTuple& color);

    // ArrowTip
    void AddArrowTip(int z, int z2, double p1_x, double p1_y, double p2_x, double p2_y, const ColorTuple& color);
};
