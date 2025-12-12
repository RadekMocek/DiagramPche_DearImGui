#include <iostream>

#include "../App.hpp"

void App::ModuleCanvas()
{
    m_parser.parse(m_source);

    std::cout << m_parser.m_result_nodes.size() << "\n";
}
