#pragma once

#include "../../../Dependency/svg.hpp"

class Exporter
{
private:
    bool m_is_enabled{};
    svg::ShapeColl m_elements;

public:
    void Start()
    {
        m_is_enabled = true;
        m_elements = svg::ShapeColl();
    }

    void AddRect(double x, double y, double width, double height)
    {
        if (!m_is_enabled) return;
        m_elements << svg::Rectangle(svg::Point(x, y), width, height, svg::Fill(svg::Color::Transparent),
                                     svg::Stroke(1, svg::Color::Red));
    }

    void Save()
    {
        if (!m_is_enabled) return;
        m_is_enabled = false;

        svg::Dimensions dimensions(500, 500);
        svg::Layout::Origin origin = svg::Layout::TopLeft;
        svg::Layout layout = svg::Layout(dimensions, origin);
        svg::Document document("test.svg", layout);

        document << m_elements;

        if (document.save()) {
            std::cout << "File saved successfully: " << std::endl;
        }
        else {
            std::cout << "Failed to save the file: " << std::endl;
        }
    }
};
