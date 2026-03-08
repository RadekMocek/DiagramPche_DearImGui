#pragma once

#include <string>

#include "imgui.h"
#include "TextEditor.h"

#define GL_SILENCE_DEPRECATION
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif

// For glad to work, if it is imported after this (which it is in this project)
#define GLFW_INCLUDE_NONE
// For native dialogs to work (fingers crossed)
#ifdef _WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#else
#define GLFW_EXPOSE_NATIVE_X11
#endif

#include <GLFW/glfw3.h>
// ReSharper disable once CppUnusedIncludeDirective
#include <glad/gl.h>
#include <nfd.h>
// ReSharper disable once CppUnusedIncludeDirective
#include <nfd_glfw3.h>

// To link with VS2010-era libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do using this pragma.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

// See 'Makefile.emscripten' for details.
#ifdef __EMSCRIPTEN__
#include "../libs/emscripten/emscripten_mainloop_stub.h"
#endif

// ReSharper disable once CppUnusedIncludeDirective
#include "Helper/Operator.hpp"
#include "Logic/SVG/Exporter.hpp"
#include "Logic/TOML/Parser.hpp"
#include "Model/CanvasNode.hpp"

// By commenting this out, Dear ImGui demo window won't be accessible from the app
#define INCLUDE_IMGUI_DEMO_WINDOW

class App
{
public:
    App() = default;
    bool Init();
    void Run();

private:
    static constexpr auto COLOR_BLACK = IM_COL32(0, 0, 0, 255);

    // Helper structs / enums =
    enum ActionAfterExport
    {
        ActionAfterExport_DoNothing, ActionAfterExport_OpenFolder, ActionAfterExport_OpenFile
    };

    enum ActionAfterUnsavedWarn
    {
        ActionAfterUnsavedWarn_Invalid,
        ActionAfterUnsavedWarn_Exit,
        ActionAfterUnsavedWarn_New,
        ActionAfterUnsavedWarn_OpenFile,
        ActionAfterUnsavedWarn_LoadExample
    };

    enum AppearanceTheme
    {
        AppearanceTheme_Light, AppearanceTheme_Dark, AppearanceTheme_Legacy
    };

    // = Members =
    GLFWwindow* m_window{};
    bool m_should_window_really_close = false;

    // GLFW alternative cursors
    bool m_is_glfw_cursor_used = false;
    GLFWcursor* m_cursor_crosshair{};

    ImFont* m_font_inconsolata_medium = nullptr;
    float m_body_split_ratio = 0.5f;

    // TOML source related
    Parser m_parser{};
    std::string m_source{};
    std::optional<std::string> m_source_filename = std::nullopt;
    bool m_is_source_dirty{}; // Dirty == edited without saving to disk
    int m_source_font_size{};

    // Text editor
    bool m_do_use_alt_editor{};
    TextEditor m_alt_editor;

    // Canvas
    bool m_do_show_grid{};
    ImVec2 m_scrolling{};
    std::unordered_map<std::string, CanvasNode> m_canvas_nodes{};
    int m_canvas_font_size{};
    float m_canvas_zoom_level{};
    // Canvas interaction
    bool m_do_show_toolbar{};
    bool m_do_show_secondary_canvas_toolbar{};
    std::optional<std::string> m_selected_or_hovered_canvas_node_key = std::nullopt;
    bool m_is_canvas_node_selected = false;
    bool m_is_dragndropping_node = false;
    NodeType m_dragndropping_node_type{};

    // Modeless
    bool m_do_show_window_demo = false;
    bool m_do_show_window_preferences = false;
    bool m_do_show_window_benchmark = false;

    // Modals
    bool m_is_queued_popup_about = false;
    bool m_is_queued_popup_export = false;
    bool m_is_queued_popup_unsavedwarn = false;
    bool m_is_queued_popup_error = false;

    // Modal business
    std::string m_modal_error_message{};
    bool m_is_action_unsavedwarn_queued = false;
    bool m_do_action_unsavedwarn_save = false;
    ActionAfterUnsavedWarn m_action_unsavedwarn_type{};
    std::string m_action_unsavedwarn_value;

    // SVG export
    Exporter m_exporter{};
    std::string m_path_export;
    int m_action_after_export_choice{};

    // Style
    AppearanceTheme m_style_current_color_theme = AppearanceTheme_Light;
    bool m_style_do_force_light_canvas{};
    ImU32 m_style_color_modal{};
    ImU32 m_style_color_secondary_toolbar{};

    // Benchmark
    bool m_is_benchmark_running = false;
    bool m_is_benchmark_first_iter = false;
    int m_bench_stats_total_nodes{};

    // = Functions =
    // Boilerplate
    static void GLFWErrorCallback(int error, const char* description);
    static void GLFWWindowCloseCallback(GLFWwindow* window);
    void Start();
    void Update();

    // App specific
    void GUIMainMenuBar();
    void GUIBody();
    void GUIToolbar(float textedit_width);
    void GUITextEditor(float textedit_width, float height);
    void GUITextEditorAlt(float textedit_width, float height);
    void GUICanvas(float height);
    void GUICanvasDrawNodes(ImDrawList* draw_list, ImVec2 origin, float node_border_offset_base);
    void GUICanvasDrawPaths(ImDrawList* draw_list, ImVec2 origin);
    void ResetCanvasScrollingAndZoom();
    void ChangeCanvasFontSizeAndZoom(int new_canvas_font_size);

    // Ghost node == semi-transparent node, used for drag'n'drop functionality, shows where the node will be placed if user releases LMB on canvas
    void GUICanvasDrawGhostNode(ImDrawList* draw_list,
                                ImVec2 mouse_pos,
                                float node_padding,
                                ImVec2 ghost_padding,
                                const char* ghost_label_c_str) const;

    // Additional windows
    void GUIWinPreferences();
    void GUIWinBenchmark();

    // All modals
    void GUIModal();

    // Widgets (if there are multiple instances of the same thing, we put it to the method to DRY)
    void WidgetTextEditorFontSizeInputInt();
    // Setter for text editor font size found in toolbar AND in preferences window
    void WidgetTextEditorPreferredCombo();
    // Combo for choosing between vanilla/3rd party text editor found in preferences window AND in start benchmark window

    // File
    // - Logic for buttons in MainMenuBar
    void HandleRegularNew();
    void HandleRegularOpen();
    void HandleRegularSave();
    void HandleOpenExample(const char* filename);
    // - Underlying logic
    void LoadSourceFromFile(const char* filename, bool is_example);
    bool SaveSourceToFile(const char* filename) const;
    void SaveSourceToFileFromDialog();
    // - "Outside of app" logic (open the file in system explorer / image viewer)
    static void ShowFileInFileManager(const std::string& filename);
    static void OpenFile(const std::string& filename);

    // Dialog
    void SetParentWindow(nfdwindowhandle_t* dialog_args_parent) const;
    std::optional<std::string> SaveSVGDialog() const;
    std::optional<std::string> SaveTOMLDialog() const;
    std::optional<std::string> SaveFileDialog(const nfdu8char_t* default_name,
                                              const nfdu8filteritem_t* filters,
                                              nfdfiltersize_t n_filters) const;
    std::optional<std::string> OpenTOMLDialog() const;

    // Source editing from canvas interaction
    std::optional<size_t> GetMSourceIdxFromSourceRegion(const toml::source_position& position);
    void ReplaceInMSource(const toml::source_region& source, const std::string& new_str);
    void InsertNodeParameterInMSource(const Node& toolbar_node, const std::string& new_str);

    // Style
    void ChangeAppearanceTheme(AppearanceTheme theme);

    // Benchmark
    void BenchmarkStart();
    void BenchmarkUpdate();
    void BenchmarkGUIUpdate();

    // Call this after we change m_source somewhere from code instead of by editing text in the text edit widget
    void OnMSourceChanged()
    {
        // Source was edited externally => source is dirty
        m_is_source_dirty = true;

        // If alt text editor is used, its text must be updated
        if (m_do_use_alt_editor) {
            //const auto cursor_pos = m_alt_editor.GetCursorPosition();
            m_alt_editor.SetText(m_source);
            //m_alt_editor.SetCursorPosition(cursor_pos);

            // This resets scroll even if I do the GetCursorPosition + SetCursorPosition ¯\_(ツ)_/¯
        }
    }

    //
    void ShowErrorModal(const std::string& error_message)
    {
        m_is_queued_popup_error = true;
        m_modal_error_message = error_message;
    }

    void SetWindowTitle(const char* title) const
    {
        glfwSetWindowTitle(m_window, title);
    }

    //???[1]: this does not work as expected
    Node DEFAULT_TOOLBAR_NODE;
};
