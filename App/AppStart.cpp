#include "App.hpp"
#include "Config.hpp"

void App::Start()
{
    // = Load Fonts =
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return a nullptr. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
    // - Read 'docs/FONTS.md' for more instructions and details. If you like the default font but want it to scale better, consider using the 'ProggyVector' from the same author!
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    // - Our Emscripten build process allows embedding fonts to be accessible at runtime from the "fonts/" folder. See Makefile.emscripten for details.
    //style.FontSizeBase = 20.0f;
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf");
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf");
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf");
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf");
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf");
    //IM_ASSERT(font != nullptr);

    const ImGuiIO& io = ImGui::GetIO();

    m_font_inconsolata_medium = io.Fonts->AddFontFromFileTTF("./Resource/Font/Inconsolata-Medium.ttf",
                                                             FONT_SIZE_DEFAULT);

    IM_ASSERT(m_font_inconsolata_medium != nullptr);

    // = Initialize all non-font members =

    m_do_show_demo_window = false;
    m_is_about_popup_queued = false;

    m_source = R"""(
[variables]
w = 110
h = 72

[[node]]
id = "cache"
value = "Cache"
xy = [20, 20]
size = ["w", "h"]

[[node]]
id = "alu"
value = "ALU"
pivot = "top"
base = ["cache", "bottom"]
xy = [0, 35]
size = ["w", "h"]

[[node]]
id = "cu"
value = "Řídící\njednotka"
pivot = "top"
base = ["alu", "bottom"]
xy = [0, 35]
size = ["w", "h"]

[[node]]
id = "datareg"
value = "Datové\nregistry"
pivot = "left"
base = ["alu", "right"]
xy = [35, 0]
size = ["w", "h"]

[[node]]
id = "statusreg"
value = "Stavové\nregistry"
pivot = "left"
base = ["cu", "right"]
xy = [35, 0]
size = ["w", "h"]
)""";
}
