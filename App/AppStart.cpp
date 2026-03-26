#include <filesystem>

#include "App.hpp"
#include "Welcome.hpp"

void App::Start()
{
    m_cursor_crosshair = glfwCreateStandardCursor(GLFW_CROSSHAIR_CURSOR);

    // = Load Fonts =
    ImGuiIO& io = ImGui::GetIO();
    constexpr auto FONT_SIZE_DEFAULT = 20.0f;
    m_font_inconsolata_medium = io.Fonts->AddFontFromFileTTF("./Resource/Font/Inconsolata-Medium.ttf",
                                                             FONT_SIZE_DEFAULT);
    IM_ASSERT(m_font_inconsolata_medium != nullptr);

    // Icon font
    ImFontConfig icons_config;
    icons_config.MergeMode = true;
    icons_config.GlyphMinAdvanceX = FONT_SIZE_DEFAULT;
    icons_config.GlyphOffset = {0.0f, 1.0f};
    io.Fonts->AddFontFromFileTTF("./Resource/Font/pictogrammers-materialdesignicons.ttf",
                                 FONT_SIZE_DEFAULT,
                                 &icons_config);

    // = App config =
    // Cannot move windows by dragging their body, only titlebar counts, just like god intended
    io.ConfigWindowsMoveFromTitleBarOnly = true;

    // = Alternative text editor =
    m_do_use_alt_editor = true;
    m_alt_editor.SetLanguageDefinition(TextEditor::LanguageDefinition::NSOLD());
    m_alt_editor.SetShowWhitespaces(false);
    m_alt_editor.SetText(WELCOME_TOML);
    // (Color palette is set elsewhere)

    // = Initialize all other members =
    m_do_show_grid = true;
    m_do_show_toolbar = true;
    m_do_show_secondary_canvas_toolbar = true;
    ResetCanvasScrollingAndZoom();

    m_path_export = (std::filesystem::current_path() / "diagram.svg").string();
    m_action_after_export_choice = ActionAfterExport_DoNothing;

    m_source = WELCOME_TOML;
    m_is_source_dirty = false;
    m_source_font_size = FONT_SIZE_DEFAULT;

    // Style
    m_style_do_force_light_canvas = true;
}
