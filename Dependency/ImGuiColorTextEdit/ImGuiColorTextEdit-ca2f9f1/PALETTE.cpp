#include "TextEditor.h"

const TextEditor::Palette& TextEditor::GetDiagramPchePalette()
{
    static constexpr Palette p = {
        {
            0xff7f7f7f, // None
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
