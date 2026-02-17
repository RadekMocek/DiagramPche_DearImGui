#pragma once

#include <queue>

#include "../../../Dependency/svg.hpp"

class Exporter
{
private:
    static constexpr auto SVG_PADDING = 50;
    static constexpr auto PRIORITY_RECT = 0;
    static constexpr auto PRIORITY_TEXT = 1;

    const svg::Stroke STROKE_BLACK = svg::Stroke(1, svg::Color::Black);
    const svg::Fill FILL_BLACK = svg::Fill(svg::Color::Black, 1.0);

    struct DrawCommand
    {
        int z;
        int same_z_priority; // I added this which should decide if the zs are equal, how to edit the operator< function ?
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
    std::priority_queue<DrawCommand> m_draw_commands;

    double m_boundaries_min_x{};
    double m_boundaries_min_y{};
    double m_boundaries_max_x{};
    double m_boundaries_max_y{};

    svg::Polyline m_polyline;

public:
    //
    void Start();
    void Save();

    // Rectangle
    void AddRect(int z, double x, double y, double width, double height,
                 const std::tuple<unsigned char, unsigned char, unsigned char, unsigned char>& color);

    // Text
    void AddText(int z, double x, double y, std::string value);

    // Line
    void StartPolyLine();

    void AddPolyLine();
};
