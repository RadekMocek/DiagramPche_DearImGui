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

class App
{
public:
    App() = default;
    bool Init();
    void Run();

private:
    // = Members =
    GLFWwindow* m_window{};
    ImVec4 m_clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    const char* m_window_title = "Dear ImGui :: Hello Counter";
    const bool m_is_dark_mode = false;

    // User state
    std::string m_source = R"""(
[[node]]
id="node_a"
value="Node A"
x=10
y=10
)""";

    // Const
    static constexpr auto c_modal_about_name = "About##modal";

    // = Functions =
    static void GLFWErrorCallback(int error, const char* description);
    void Start();
    void Update();

    // User functions
    void ModuleMain();
    void ModuleTextEditor();
};
