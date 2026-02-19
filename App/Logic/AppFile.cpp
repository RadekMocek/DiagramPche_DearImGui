#include <filesystem>

#include "../App.hpp"
#include "../Config.hpp"

#ifdef _WIN32
#include <shlobj.h>
#endif

void App::LoadSourceFromFile(const char* filename)
{
    if (std::ifstream stream(filename, std::ios::in | std::ios::binary); stream) {
        stream.seekg(0, std::ios::end);
        m_source.resize(stream.tellg());
        stream.seekg(0, std::ios::beg);
        stream.read(&m_source[0], static_cast<std::streamsize>(m_source.size()));
        stream.close();
    }

    std::erase(m_source, '\r'); // \r\n → \n

    m_scrolling = SCROLLING_DEFAULT;
}

void App::ShowFileInFileManager(const std::string& filename)
{
    const auto absolute_path = std::filesystem::absolute(filename);
#ifdef _WIN32
    PIDLIST_ABSOLUTE pidl = nullptr;
    if (SUCCEEDED(SHParseDisplayName(absolute_path.wstring().c_str(), nullptr, &pidl, 0, nullptr))) {
        // ReSharper disable once CppFunctionResultShouldBeUsed
        SHOpenFolderAndSelectItems(pidl, 0, nullptr, 0);
        CoTaskMemFree(pidl);
    }
#else
    // This opens new window everytime, even if window with same path already exists.
    // It also cannot select the file in the folder.
    // I could call something like "nemo {absolute_path} &", but because of the '&' at the end,
    // I cannot get the error code to check if nemo is not installed and use some fallback option.
    // (Without the '&' it freezes my app until nemo is closed)
    // Using fork-exec is probably the solution to this problem, but I'm not going to that rabbit hole right now.
    std::system(std::format("xdg-open {}", absolute_path.parent_path().string()).c_str());
#endif
}

void App::OpenFile(const std::string& filename)
{
    const auto absolute_path = std::filesystem::absolute(filename);
#ifdef _WIN32
    ShellExecuteW(nullptr, L"open", absolute_path.wstring().c_str(), nullptr, nullptr, SW_SHOWNORMAL);
#else
    std::system(std::format("xdg-open {}", absolute_path.string()).c_str());
#endif
}
