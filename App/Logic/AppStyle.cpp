#include "../App.hpp"

void App::ChangeAppearanceTheme(const AppearanceTheme theme)
{
    constexpr auto COLOR_MODAL_LIGHT = IM_COL32(240, 240, 240, 255);
    constexpr auto COLOR_MODAL_DARK = IM_COL32(14, 14, 14, 255);
    constexpr auto COLOR_MODAL_LEGACY = IM_COL32(0, 0, 0, 255);

    constexpr auto COLOR_SECONDARY_TOOLBAR_LIGHT = IM_COL32(219, 219, 219, 255);
    constexpr auto COLOR_SECONDARY_TOOLBAR_DARK = IM_COL32(36, 36, 36, 255);
    constexpr auto COLOR_SECONDARY_TOOLBAR_LEGACY = IM_COL32(82, 82, 112, 255);

    switch (theme) {
    case AppearanceTheme_Light:
        ImGui::StyleColorsLight();
        m_alt_editor.SetPalette(TextEditor::GetDiagramPchePaletteLight());
        m_style_color_modal = COLOR_MODAL_LIGHT;
        m_style_color_secondary_toolbar = COLOR_SECONDARY_TOOLBAR_LIGHT;
        break;

    case AppearanceTheme_Dark:
        ImGui::StyleColorsDark();
        m_alt_editor.SetPalette(TextEditor::GetDiagramPchePaletteDark());
        m_style_color_modal = COLOR_MODAL_DARK;
        m_style_color_secondary_toolbar = COLOR_SECONDARY_TOOLBAR_DARK;
        break;

    case AppearanceTheme_Legacy:
        ImGui::StyleColorsClassic();
        m_alt_editor.SetPalette(TextEditor::GetDiagramPchePaletteLegacy());
        m_style_color_modal = COLOR_MODAL_LEGACY;
        m_style_color_secondary_toolbar = COLOR_SECONDARY_TOOLBAR_LEGACY;
        break;
    }

    m_style_current_color_theme = theme;
}
