#pragma once

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include "TextEditor.h"

#include <cstdio>
#include <string>
#include <iostream>

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

#include <glad/gl.h>

#include <nfd.h>
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

    // = Members =
    GLFWwindow* m_window{};
    const ImVec4 m_clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    ImFont* m_font_inconsolata_medium = nullptr;

    const char* m_window_title = "Untitled – DiagramPche :: Dear ImGui";
    const bool m_is_dark_mode = false;

    // Body
    std::string m_source{};
    float m_body_split_ratio = 0.5f;
    Parser m_parser{};

    // Text editor
    bool m_do_use_alt_editor = true;
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
    bool m_is_queued_popup_error = false;
    std::string m_modal_error_message{};

    Exporter m_exporter{};
    std::string m_path_export;
    int m_action_after_export_choice{};

    // = Functions =
    // Boilerplate
    static void GLFWErrorCallback(int error, const char* description);
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
    void LoadSourceFromFile(const char* filename);
    static void ShowFileInFileManager(const std::string& filename);
    static void OpenFile(const std::string& filename);
    // Dialog
    void SetParentWindow(nfdwindowhandle_t* dialog_args_parent) const;
    std::optional<std::string> SaveSVGDialog() const;
    std::optional<std::string> SaveFileDialog(const nfdu8char_t* default_name,
                                              const nfdu8filteritem_t* filters,
                                              nfdfiltersize_t n_filters) const;

    //
    void ShowErrorModal(const std::string& error_message)
    {
        m_is_queued_popup_error = true;
        m_modal_error_message = error_message;
    }
};
