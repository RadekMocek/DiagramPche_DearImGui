#include "../App.hpp"
#include "../Config.hpp"

void App::LoadSourceFromFile(const char* filename)
{
    if (std::ifstream stream(filename, std::ios::in | std::ios::binary); stream) {
        stream.seekg(0, std::ios::end);
        m_source.resize(stream.tellg());
        stream.seekg(0, std::ios::beg);
        stream.read(&m_source[0], static_cast<std::streamsize>(m_source.size()));
        stream.close();
    }

    m_scrolling = SCROLLING_DEFAULT;
}
