#include "imgui-app.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#define GL_SILENCE_DEPRECATION
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include <GLFW/glfw3.h>  // Will drag system OpenGL headers

#include "color.h"
#include "imgui-dialog.h"
#include "imgui-window.h"

#ifndef GL_FRAMEBUFFER_SRGB
#define GL_FRAMEBUFFER_SRGB 0x8DB9
#endif

namespace {
static void glfw_error_callback(int error, const char* description) {
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

const bool use_srgb_framebuffer = true;
}  // namespace

ImGuiApp::ImGuiApp(const std::string& name) : _name(name) {
    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit()) return;

        // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100
    const char* glsl_version = "#version 100";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
    // GL 3.2 + GLSL 150
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif
    glfwWindowHint(GLFW_SRGB_CAPABLE, use_srgb_framebuffer);

    // Create window with graphics context
    _glfw_window = glfwCreateWindow(1280, 720, _name.c_str(), NULL, NULL);
    if (_glfw_window == NULL) return;
    glfwMakeContextCurrent(_glfw_window);
    glfwSwapInterval(1);  // Enable vsync
    if (use_srgb_framebuffer) {
        glEnable(GL_FRAMEBUFFER_SRGB);
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    ImFontConfig ifc;
    ifc.OversampleH = 1;
    ifc.OversampleV = 1;
    ifc.SizePixels = 15.0f;
    ifc.PixelSnapH = false;
    
    io.Fonts->AddFontFromFileTTF("C:/Windows/Fonts/arial.ttf", ifc.SizePixels, &ifc);

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    ImGui::GetStyle().WindowRounding = 8.0f;

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(_glfw_window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
}

ImGuiApp::~ImGuiApp() {
    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(_glfw_window);
    glfwTerminate();
}

void ImGuiApp::run() {
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    if (use_srgb_framebuffer) {
        clear_color = srgbToLinear(clear_color);
    }

    while (!glfwWindowShouldClose(_glfw_window)) {
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        for (auto& window: _windows) {
            if (ImGui::Begin(window->name().c_str())) {
                window->doit();
            }
            ImGui::End();
        }

        for (auto& dialog: _dialogs) {
            if (ImGui::BeginPopupModal(dialog->name().c_str(), 0, ImGuiWindowFlags_AlwaysAutoResize)) {
                dialog->doit();
                ImGui::EndPopup();
            }
        }

        for (auto& event: _events) {
            event();
        }

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(_glfw_window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        if (use_srgb_framebuffer) {
            imguiAdjustSrgb();
        }
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(_glfw_window);
    }
}

void ImGuiApp::addWindow(std::shared_ptr<ImGuiWindowBase> window) {
    _windows.push_back(window);
}

void ImGuiApp::addDialog(std::shared_ptr<ImGuiDialog> dialog) {
    _dialogs.push_back(dialog);
}

void ImGuiApp::addEvent(std::function<void()> event) {
    _events.push_back(event);
}


void ImGuiApp::setStyle(bool dark) {
    if (dark) {
        ImGui::StyleColorsDark();
    } else {
        ImGui::StyleColorsLight();
    }
}
