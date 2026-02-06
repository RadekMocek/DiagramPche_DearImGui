#pragma once

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include <cstdio>
#include <string>

#define GL_SILENCE_DEPRECATION
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/gl.h>

// To link with VS2010-era libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do using this pragma.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

// See 'Makefile.emscripten' for details.
#ifdef __EMSCRIPTEN__
#include "../libs/emscripten/emscripten_mainloop_stub.h"
#endif

#include "Logic/Parser.hpp"
#include "Model/CanvasNode.hpp"

class App
{
public:
    App() = default;
    bool Init();
    void Run();

private:
    // = Members =
    GLFWwindow* m_window{};
    const ImVec4 m_clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    ImFont* m_font_inconsolata_medium = nullptr;

    const char* m_window_title = "Untitled – DiagramPche :: Dear ImGui";
    const bool m_is_dark_mode = false;

    bool m_do_show_demo_window{};
    bool m_is_about_popup_queued{};

    std::string m_source{};

    Parser m_parser{};

    std::unordered_map<std::string, CanvasNode> m_canvas_nodes{};

    // = Functions =
    // Boilerplate
    static void GLFWErrorCallback(int error, const char* description);
    void Start();
    void Update();

    // App specific
    void GUIMainMenuBar();
    void GUIBody();
    void GUITextEditor();

    void GUICanvas();
    void GUICanvasDrawNodes(ImDrawList* draw_list, ImVec2 origin, float zoom_level, int font_size);
    void GUICanvasDrawPaths(ImDrawList* draw_list, ImVec2 origin, float zoom_level);

    // (GUICanvasDrawPaths helper method)
    static inline void PathShift(ImVec2& point, bool& is_arrow_satisfied, const Path& path, const Pivot& pivot,
                                 ImDrawList* draw_list, ImVec2 origin, float zoom_level, ImU32 color);

    void LoadSourceFromFile(const char* filename);
};
