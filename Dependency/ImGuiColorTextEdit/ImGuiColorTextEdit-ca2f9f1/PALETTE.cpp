#include "TextEditor.h"

const TextEditor::Palette& TextEditor::GetDiagramPchePaletteLight()
{
    static constexpr Palette p = {
        {
            0xff7f7f7f, // Default
            IM_COL32(55, 61, 194, 255), // Keyword
            0xff2020a0, // Number
            IM_COL32(77, 89, 1, 255), // String
            0xff304070, // Char literal
            0xff000000, // Punctuation
            0xff406060, // Preprocessor
            0xff404040, // Identifier
            IM_COL32(16, 127, 118, 255), // Known identifier
            0xffc040a0, // Preproc identifier
            IM_COL32(135, 148, 147, 255), // Comment (single line)
            IM_COL32(135, 148, 147, 255), // Comment (multi line)
            IM_COL32(255, 255, 255, 255), // Background
            0xff000000, // Cursor
            0x40000000, // Selection
            IM_COL32(211, 1, 2, 80), // ErrorMarker
            0x80f08000, // Breakpoint
            IM_COL32(135, 148, 147, 255), // Line number
            IM_COL32(216, 216, 216, 64), // Current line fill
            IM_COL32(240, 240, 240, 0), // Current line fill (inactive)
            0x40000000, // Current line edge
        }
    };
    return p;
}

// Inspired by https://docs.rs/egui_code_editor/0.2.21/egui_code_editor/struct.ColorTheme.html#associatedconstant.SONOKAI
// (https://github.com/sainnhe/sonokai)
// (https://github.com/p4ymak/egui_code_editor)
const TextEditor::Palette& TextEditor::GetDiagramPchePaletteDark()
{
    static constexpr Palette p = {
        {
            IM_COL32(226, 226, 227, 255), // Default
            IM_COL32(252, 93, 124, 255), // Keyword
            IM_COL32(179, 157, 243, 255), // Number
            IM_COL32(231, 198, 100, 255), // String
            0xff70a0e0, // Char literal
            IM_COL32(127, 132, 144, 255), // Punctuation
            0xff408080, // Preprocessor
            IM_COL32(226, 226, 227, 255), // Identifier
            IM_COL32(57, 158, 230, 255), // Known identifier
            0xffc040a0, // Preproc identifier
            IM_COL32(127, 132, 144, 255), // Comment (single line)
            IM_COL32(127, 132, 144, 255), // Comment (multi line)
            0xff101010, // Background
            0xffe0e0e0, // Cursor
            IM_COL32(68, 72, 82, 255), // Selection
            0x800020ff, // ErrorMarker
            0x40f08000, // Breakpoint
            0xff707000, // Line number
            0x40000000, // Current line fill
            0x40808080, // Current line fill (inactive)
            0x40a0a0a0, // Current line edge
        }
    };
    return p;
}

const TextEditor::Palette& TextEditor::GetDiagramPchePaletteLegacy()
{
    static constexpr Palette p = {
        {
            0xff00ffff, // Default
            0xffffff00, // Keyword
            IM_COL32(179, 157, 243, 255), // Number
            IM_COL32(231, 198, 100, 255), // String
            0xff808000, // Char literal
            0xff808000, // Punctuation
            0xff008000, // Preprocessor
            0xffffffff, // Identifier
            IM_COL32(254, 219, 169, 255), // Known identifier
            0xffff00ff, // Preproc identifier
            IM_COL32(0, 128, 0, 255), // Comment (single line)
            IM_COL32(0, 128, 0, 255), // Comment (multi line)
            IM_COL32(1, 39, 86, 255), // Background
            0xff0080ff, // Cursor
            IM_COL32(254, 219, 169, 64), // Selection
            0xa00000ff, // ErrorMarker
            0x80ff8000, // Breakpoint
            0xff808000, // Line number
            0x40000000, // Current line fill
            0x40808080, // Current line fill (inactive)
            0x40000000, // Current line edge
        }
    };
    return p;
}
