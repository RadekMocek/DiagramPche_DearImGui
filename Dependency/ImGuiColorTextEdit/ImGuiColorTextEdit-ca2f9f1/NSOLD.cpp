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
        constexpr std::pair<std::string_view, std::string_view> identifiers[] = {
            {"value", "Value of the node"},
            {"xy", "Position of the node"},
            {"pivot", "Node's pivot"},
            {"color", "Color of the node or path"},
            {"size", "Size of the node"},
            {"label_pos", "Position of the node's label"},
            {"z", "Z-depth value"},
            {"start", "Position of the path's starting point"},
            {"end", "Position of the path's ending point"},
            {"ends", "Positions of all of the path's ending points"},
            {"points", "Path points between start and end(s)"},
            {"shift", "Offset of path's start and end points"},
            {"tips", "Appearence of path's ends"},
        };
        for (auto [name, decl] : identifiers) {
            Identifier id;
            id.mDeclaration = decl;
            langDef.mIdentifiers.insert(std::make_pair(std::string(name), id));
        }

        langDef.mTokenRegexStrings.push_back(
            std::make_pair<std::string, PaletteIndex>("\\\"(\\\\.|[^\\\"])*\\\"", PaletteIndex::String));

        langDef.mTokenRegexStrings.push_back(
            std::make_pair<std::string, PaletteIndex>("\\\'[^\\\']*\\\'", PaletteIndex::String));

        langDef.mTokenRegexStrings.push_back(
            std::make_pair<std::string, PaletteIndex>("[+-]?([0-9]+([.][0-9]*)?|[.][0-9]+)", PaletteIndex::Number));

        langDef.mTokenRegexStrings.push_back(
            std::make_pair<std::string, PaletteIndex>("[a-zA-Z_][a-zA-Z0-9_]*", PaletteIndex::Identifier));

        langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>(
            "[\\[\\]\\{\\}\\!\\%\\^\\&\\*\\(\\)\\-\\+\\=\\~\\|\\<\\>\\?\\/\\;\\,\\.]", PaletteIndex::Punctuation));

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
