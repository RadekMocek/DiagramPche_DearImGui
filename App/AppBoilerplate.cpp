#include "App.hpp"

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../Dependency/stb_image.h"

#ifdef _WIN32
#include <windows.h>
#endif

void App::GLFWErrorCallback(const int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

bool App::Init()
{
#ifdef _WIN32
    // Initializing the COM library for use by this thread so I can use `SHParseDisplayName` and `SHOpenFolderAndSelectItems` later.
    // With those functions I can open explorer with highlighted item (1) and reuse existing explorer window if it already has same path opened (2).
    // I was able to come up with simplier solutions, but they were capable of either (1) or (2), not both.
    // Without calling this, it still works, but docs say you have to call this, so idk.
    // Docs also say you have to uninit this later, so I do that at the end of `App::Run()`.
    if (const HRESULT hr_com_init = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED); hr_com_init != S_OK) {
        std::cerr << "CoInitializeEx != S_OK\n";
    }
#endif

    std::cout << "RAM OK\n";
    std::cout << "ROM OK\n";

    glfwSetErrorCallback(GLFWErrorCallback);
    if (!glfwInit()) {
        std::cerr << "!glfwInit()\n";
        return false;
    }
    std::cout << "GLFW OK\n";

    // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100 (WebGL 1.0)
    const char* glsl_version = "#version 100";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(IMGUI_IMPL_OPENGL_ES3)
    // GL ES 3.0 + GLSL 300 es (WebGL 2.0)
    const char* glsl_version = "#version 300 es";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
    // GL 3.2 + GLSL 150
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Required on Mac
#else
    // GL 3.0 + GLSL 130
    const auto glsl_version = "#version 330";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // 3.0+ only
#endif

    // Create window with graphics context
    const float main_scale = ImGui_ImplGlfw_GetContentScaleForMonitor(glfwGetPrimaryMonitor());
    // Valid on GLFW 3.3+ only
    m_window = glfwCreateWindow(static_cast<int>(1280 * main_scale),
                                static_cast<int>(800 * main_scale),
                                "Untitled – DiagramPche :: Dear ImGui",
                                nullptr,
                                nullptr);
    if (m_window == nullptr) {
        return false;
    }

    glfwMakeContextCurrent(m_window);
    glfwSwapInterval(1); // Enable vsync

    GLFWimage images[1];
    images[0].pixels = stbi_load("./Resource/Icon/icon-256.png", &images[0].width, &images[0].height, nullptr, 4);
    glfwSetWindowIcon(m_window, 1, images);
    stbi_image_free(images[0].pixels);

    // Init Glad
    if (!gladLoaderLoadGL()) {
        return false;
    }
    std::cout << "GLAD OK\n";

    // Native dialogs
    if (NFD_Init() != NFD_OKAY) {
        std::cerr << "NFD_Init() != NFD_OKAY :: " << NFD_GetError() << '\n';
        return false;
    }
    std::cout << "NFDE OK\n";

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls

    // Setup Dear ImGui style
    // ReSharper disable once CppDFAConstantConditions
    if (m_is_dark_mode) {
        //ImGui::StyleColorsDark();
    }
    else {
        ImGui::StyleColorsLight();
    }

    // Setup scaling
    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(main_scale);
    // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
    style.FontScaleDpi = main_scale;
    // Set initial font scale. (using io.ConfigDpiScaleFonts=true makes this unnecessary. We leave both here for documentation purpose)

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(m_window, true);
#ifdef __EMSCRIPTEN__
    ImGui_ImplGlfw_InstallEmscriptenCallbacks(m_window, "#canvas");
#endif
    ImGui_ImplOpenGL3_Init(glsl_version);

    std::cout << "DEAR IMGUI OK\n";

    return true;
}

void App::Run()
{
    constexpr auto clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // User start
    Start();

    // Main loop
#ifdef __EMSCRIPTEN__
    // For an Emscripten build we are disabling file-system access, so let's not attempt to do a fopen() of the imgui.ini file.
    // You may manually call LoadIniSettingsFromMemory() to load settings from your own storage.
    io.IniFilename = nullptr;
    EMSCRIPTEN_MAINLOOP_BEGIN
#else
    while (!glfwWindowShouldClose(m_window))
#endif
    {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        glfwPollEvents();
        if (glfwGetWindowAttrib(m_window, GLFW_ICONIFIED) != 0) {
            ImGui_ImplGlfw_Sleep(10);
            continue;
        }

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // User update
        Update();

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(m_window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x * clear_color.w,
                     clear_color.y * clear_color.w,
                     clear_color.z * clear_color.w,
                     clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(m_window);
    }
#ifdef __EMSCRIPTEN__
    EMSCRIPTEN_MAINLOOP_END;
#endif

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    NFD_Quit();

    glfwDestroyWindow(m_window);
    glfwTerminate();

#ifdef _WIN32
    // See begin of `App::Init()`.
    CoUninitialize();
#endif
}
