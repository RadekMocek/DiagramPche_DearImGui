#include "TextEditor.h"

const TextEditor::LanguageDefinition& TextEditor::LanguageDefinition::NSOLD()
{
    static bool inited = false;
    static LanguageDefinition langDef;
    if (!inited) {
        //
        constexpr std::string_view keywords[] = {
            "variables", "node", "path"
        };
        for (const auto k : keywords) {
            langDef.mKeywords.insert(std::string(k));
        }
        //
        constexpr std::string_view identifiers[] = {
            "color",
            "color_border",
            "end",
            "ends",
            "label",
            "label_bg",
            "label_pos",
            "label_shift",
            "pivot",
            "points",
            "shift",
            "size",
            "start",
            "tips",
            "type",
            "value",
            "xy",
            "z",
        };
        for (auto i : identifiers) {
            Identifier id;
            langDef.mIdentifiers.insert(std::make_pair(std::string(i), id));
        }

        langDef.mTokenRegexStrings.push_back(
            std::make_pair<std::string, PaletteIndex>(R"(\"(\\.|[^\"])*\")", PaletteIndex::String));

        langDef.mTokenRegexStrings.push_back(
            std::make_pair<std::string, PaletteIndex>(R"(\'[^\']*\')", PaletteIndex::String));

        langDef.mTokenRegexStrings.push_back(
            std::make_pair<std::string, PaletteIndex>("[+-]?([0-9]+([.][0-9]*)?|[.][0-9]+)", PaletteIndex::Number));

        langDef.mTokenRegexStrings.push_back(
            std::make_pair<std::string, PaletteIndex>("[a-zA-Z_][a-zA-Z0-9_]*", PaletteIndex::Identifier));

        langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>(
            R"([\[\]\{\}\!\%\^\&\*\(\)\-\+\=\~\|\<\>\?\/\;\,\.])", PaletteIndex::Punctuation));

        // TOML does not allow multiline comments but this has to be set to something
        langDef.mCommentStart = "/*";
        langDef.mCommentEnd = "*/";

        langDef.mSingleLineComment = "#";

        langDef.mCaseSensitive = true;
        langDef.mAutoIndentation = false;

        langDef.mName = "NSOLD";

        inited = true;
    }
    return langDef;
}
