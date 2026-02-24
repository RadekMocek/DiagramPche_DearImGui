#include <filesystem>

#include "App.hpp"
#include "Config.hpp"

void App::Start()
{
    // = Load Fonts =
    // - If fonts are not explicitly loaded, Dear ImGui will select an embedded font: either AddFontDefaultVector() or AddFontDefaultBitmap().
    //   This selection is based on (style.FontSizeBase * style.FontScaleMain * style.FontScaleDpi) reaching a small threshold.
    // - You can load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - If a file cannot be loaded, AddFont functions will return a nullptr. Please handle those errors in your code (e.g. use an assertion, display an error and quit).
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use FreeType for higher quality font rendering.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    // - Our Emscripten build process allows embedding fonts to be accessible at runtime from the "fonts/" folder. See Makefile.emscripten for details.
    //style.FontSizeBase = 20.0f;
    //io.Fonts->AddFontDefaultVector();
    //io.Fonts->AddFontDefaultBitmap();
    //io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf");
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf");
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf");
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf");
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf");
    //IM_ASSERT(font != nullptr);

    ImGuiIO& io = ImGui::GetIO();

    m_font_inconsolata_medium = io.Fonts->AddFontFromFileTTF("./Resource/Font/Inconsolata-Medium.ttf",
                                                             FONT_SIZE_DEFAULT);

    IM_ASSERT(m_font_inconsolata_medium != nullptr);

    // = App config =
    // Cannot move windows by dragging their body, only titlebar counts, just like god intended
    io.ConfigWindowsMoveFromTitleBarOnly = true;

    // = Alternative text editor =
    const auto alt_editor_language = TextEditor::LanguageDefinition::NSOLD();
    m_alt_editor.SetLanguageDefinition(alt_editor_language);
    m_alt_editor.SetPalette(TextEditor::GetDiagramPchePalette());
    m_alt_editor.SetShowWhitespaces(false);
    m_alt_editor.SetTabSize(1);
    m_alt_editor.SetText(WELCOME_TOML);

    // = Initialize all other members =
    m_do_show_grid = true;
    ResetCanvasScrollingAndZoom();

    m_path_export = (std::filesystem::current_path() / "diagram.svg").string();
    m_action_after_export_choice = ActionAfterExport_DoNothing;

    m_source = WELCOME_TOML;
}

void App::ResetCanvasScrollingAndZoom()
{
    m_scrolling = SCROLLING_DEFAULT;
    m_canvas_font_size = CANVAS_FONT_SIZE_BASE;
    m_canvas_zoom_level = 1.0f;
}
