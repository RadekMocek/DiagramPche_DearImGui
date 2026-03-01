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

#include "Logic/SVG/Exporter.hpp"
#include "Logic/TOML/Parser.hpp"
#include "Model/CanvasNode.hpp"

class App
{
public:
    App() = default;
    bool Init();
    void Run();

private:
    // Helper structs / enums =
    enum ActionAfterExport
    {
        ActionAfterExport_DoNothing, ActionAfterExport_OpenFolder, ActionAfterExport_OpenFile
    };

    enum ActionAfterUnsavedWarn
    {
        ActionAfterUnsavedWarn_Invalid,
        ActionAfterUnsavedWarn_New,
        ActionAfterUnsavedWarn_OpenFile,
        ActionAfterUnsavedWarn_LoadExample
    };

    // = Members =
    GLFWwindow* m_window{};
    bool m_should_window_really_close = false;
    GLFWcursor* m_cursor_crosshair;

    ImFont* m_font_inconsolata_medium = nullptr;
    const bool m_is_dark_mode = false;
    float m_body_split_ratio = 0.5f;

    // TOML source related
    Parser m_parser{};
    std::string m_source{};
    std::optional<std::string> m_source_filename = std::nullopt;
    bool m_is_source_dirty{}; // Dirty == edited without saving to disk

    // Text editor
    bool m_do_use_alt_editor{};
    TextEditor m_alt_editor;

    // Canvas
    bool m_do_show_grid{};
    ImVec2 m_scrolling{};
    std::unordered_map<std::string, CanvasNode> m_canvas_nodes{};
    int m_canvas_font_size{};
    float m_canvas_zoom_level{};

    // Modeless
    bool m_do_show_window_demo = false;
    bool m_do_show_window_preferences = false;

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

    // = Functions =
    // Boilerplate
    static void GLFWErrorCallback(int error, const char* description);
    //static void GLFWWindowCloseCallback(GLFWwindow* window);
    void Start();
    void Update();

    // App specific
    void GUIMainMenuBar();
    void GUIBody();
    void GUITextEditor(float textedit_width);
    void GUITextEditorAlt(float textedit_width);

    void GUICanvas();
    void GUICanvasDrawNodes(ImDrawList* draw_list, ImVec2 origin, float zoom_level, int font_size);
    void GUICanvasDrawPaths(ImDrawList* draw_list, ImVec2 origin, float zoom_level);
    void ResetCanvasScrollingAndZoom();

    void GUIWinPreferences();

    void GUIModal();

    // File
    // - Logic for buttons in MainMenuBar
    void HandleRegularNew();
    void HandleRegularOpen();
    void HandleRegularSave();
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
};
