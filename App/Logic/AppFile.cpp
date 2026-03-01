#include <filesystem>

#include "../App.hpp"
#include "../Config.hpp"

#ifdef _WIN32
#include <shlobj.h>
#endif

// == Logic for buttons in MainMenuBar ==========================================

void App::HandleRegularNew()
{
    m_source.clear();
    m_alt_editor.SetText(m_source);
    m_source_filename = std::nullopt;
    m_is_source_dirty = false;
}

void App::HandleRegularOpen()
{
    if (const auto path = OpenTOMLDialog(); path.has_value()) {
        LoadSourceFromFile(path.value().c_str(), false);
    }
}

void App::HandleRegularSave()
{
    if (!m_source_filename.has_value()) {
        SaveSourceToFileFromDialog();
    }
    else {
        if (SaveSourceToFile(m_source_filename.value().c_str())) {
            m_is_source_dirty = false;
        }
    }
}

// == Underlying logic ==========================================================

void App::LoadSourceFromFile(const char* filename, const bool is_example)
{
    if (std::ifstream stream(filename, std::ios::in | std::ios::binary); stream) {
        stream.seekg(0, std::ios::end);
        m_source.resize(stream.tellg());
        stream.seekg(0, std::ios::beg);
        stream.read(&m_source[0], static_cast<std::streamsize>(m_source.size()));
        stream.close();
    }

    std::erase(m_source, '\r'); // \r\n → \n

    m_alt_editor.SetText(m_source);

    ResetCanvasScrollingAndZoom();

    m_source_filename = (is_example) ? std::nullopt : std::optional<std::string>{filename};
    m_is_source_dirty = false;
}

bool App::SaveSourceToFile(const char* filename) const
{
    /*if (m_do_use_alt_editor) {
        m_source = m_alt_editor.GetText(); // This gets called every frame so shouldn't be needed to call here as well
    }*/
    if (std::ofstream file(filename); file.is_open()) {
        file << m_source;
        return true;
    }
    return false;
    // file.close() should be called automatically out of scope
}

void App::SaveSourceToFileFromDialog()
{
    if (const auto path = SaveTOMLDialog(); path.has_value()) {
        if (SaveSourceToFile(path.value().c_str())) {
            m_source_filename = path;
            m_is_source_dirty = false;
        }
    }
}

// == "Outside of app" logic (open the file in system explorer / image viewer) ==

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
    if (access("/usr/bin/nemo", X_OK) == 0) {
        // Nemo is the Cinnamon's default file manager, by calling this instead of `xdg-open`, it highlights the file.
        // Sadly, it still opens new window every time, even when window with that path already exists.
        std::system(std::format("nemo --existing-window {} >/dev/null 2>&1 &", absolute_path.string()).c_str());
        // This could be done for every file manager, but I only use Mint right now so there's no way of testing it.
    }
    else {
        // This opens new window everytime, even if window with same path already exists. It also can't select the file in the folder.
        // `xdg-utils` is usually installed so this is a good fallback option.
        std::system(std::format("xdg-open {} >/dev/null 2>&1", absolute_path.parent_path().string()).c_str());
    }
#endif
}

void App::OpenFile(const std::string& filename)
{
    const auto absolute_path = std::filesystem::absolute(filename);
#ifdef _WIN32
    ShellExecuteW(nullptr, L"open", absolute_path.wstring().c_str(), nullptr, nullptr, SW_SHOWNORMAL);
#else
    std::system(std::format("xdg-open {} >/dev/null 2>&1", absolute_path.string()).c_str());
#endif
}
