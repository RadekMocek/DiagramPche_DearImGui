#include "../App.hpp"

void App::SetParentWindow(nfdwindowhandle_t* dialog_args_parent) const
{
    if (!NFD_GetNativeWindowFromGLFWWindow(m_window, dialog_args_parent)) {
        std::cerr << "NFD_GetNativeWindowFromGLFWWindow failed\n";
    }
}

std::optional<std::string> App::SaveSVGDialog() const
{
    constexpr auto default_name = "diagram.svg";
    constexpr int n_filters = 1;
    constexpr nfdu8filteritem_t filters[n_filters] = {{"Scalable Vector Graphics", "svg"}};
    return SaveFileDialog(default_name, filters, n_filters);
}

std::optional<std::string> App::SaveTOMLDialog() const
{
    constexpr auto default_name = "diagram.toml";
    constexpr int n_filters = 1;
    constexpr nfdu8filteritem_t filters[n_filters] = {{"Diagram", "toml"}};
    return SaveFileDialog(default_name, filters, n_filters);
}

std::optional<std::string> App::SaveFileDialog(const nfdu8char_t* default_name,
                                               const nfdu8filteritem_t* filters,
                                               const nfdfiltersize_t n_filters) const
{
    nfdsavedialogu8args_t args = {nullptr};
    SetParentWindow(&args.parentWindow);
    args.defaultName = default_name;
    args.filterList = filters;
    args.filterCount = n_filters;

    char* path;

    switch (NFD_SaveDialogU8_With(&path, &args)) {
    case NFD_OKAY:
        {
            std::string result(path);
            NFD_FreePathU8(path);
            return result;
        }
    case NFD_CANCEL:
        break;
    case NFD_ERROR:
        std::cerr << "NFD_SaveDialogU8_With error: " << NFD_GetError() << '\n';
        break;
    default:
        break;
    }

    return std::nullopt;
}

std::optional<std::string> App::OpenTOMLDialog() const
{
    constexpr int n_filters = 1;
    constexpr nfdu8filteritem_t filters[n_filters] = {{"Diagram", "toml"}};

    nfdopendialogu8args_t args = {nullptr};
    SetParentWindow(&args.parentWindow);
    args.filterList = filters;
    args.filterCount = n_filters;

    char* path;

    switch (NFD_OpenDialogU8_With(&path, &args)) {
    case NFD_OKAY:
        {
            std::string result(path);
            NFD_FreePathU8(path);
            return result;
        }
    case NFD_CANCEL:
        break;
    case NFD_ERROR:
        std::cerr << "NFD_OpenDialogU8_With error: " << NFD_GetError() << '\n';
        break;
    default:
        break;
    }

    return std::nullopt;
}
