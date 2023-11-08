#include "imgui-app.h"

#include <chrono>
#include <filesystem>
#include <iostream>

#if defined(_WIN32)
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shellapi.h>
#include <shlobj.h>
#endif

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>
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

class Timer {
    using clock = std::chrono::steady_clock;

   public:
    Timer() {
        m_start = clock::now();
    }
    unsigned long stop() {
        auto end = clock::now();
        auto diff = std::chrono::duration_cast<std::chrono::microseconds>(end - m_start);
        return (unsigned long)diff.count();
    }

   private:
    std::chrono::time_point<clock> m_start;
};

class RollingAverageIsh {
   public:
    RollingAverageIsh() : m_average(0) {}

    void sample(double value) {
        m_average = m_average + (value - m_average) / double(N);
    }

    double get() const {
        return m_average;
    }

   private:
    double m_average;
    static const int N = 30;
};

std::filesystem::path getAppDataPath() {
#if defined(_WIN32)
    TCHAR szPath[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, szPath))) {
        std::filesystem::path appdata(szPath);
        appdata /= "Bollbok";
        std::filesystem::create_directories(appdata);
        return appdata;
    }
#elif __linux__
    const char* home = getenv("HOME");
    if (home) {
        return std::string(home) + u8"/.bollbok";
    }
#else
#error "Unsupported environment"
#endif
    return "";
}

}  // namespace

ImGuiApp::ImGuiApp(const std::string& name) : _name(name), _wantsToQuit(false), _quit(false) {
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
    _glfw_window = glfwCreateWindow(1400, 1200, _name.c_str(), NULL, NULL);
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

    std::filesystem::path appdata = getAppDataPath();
    // std::filesystem::remove(appdata / "imgui.ini");
    io.IniFilename = ImStrdup((appdata / "imgui.ini").u8string().c_str());

    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    ImFontConfig ifc;
    ifc.OversampleH = 1;
    ifc.OversampleV = 1;
    ifc.SizePixels = 23.0f;
    ifc.PixelSnapH = false;

    io.Fonts->AddFontFromFileTTF("C:/Windows/Fonts/arial.ttf", ifc.SizePixels, &ifc);

    // Setup Dear ImGui style
    // ImGui::StyleColorsDark();
    ImGui::StyleColorsClassic();
    ImGui::GetStyle().WindowRounding = 4.0f;

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(_glfw_window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
    _settings.init();

    ImGui::LoadIniSettingsFromDisk(ImGui::GetCurrentContext()->IO.IniFilename);
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

    std::vector<RollingAverageIsh> window_timings(_windows.size());
    while (!_quit) {
        if (_wantsToQuit = glfwWindowShouldClose(_glfw_window)) {
            glfwSetWindowShouldClose(_glfw_window, 0);
        }
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        try {
            _menu.doit();

            int index = 0;
            float width = ImGui::CalcTextSize("1 300.23 kr").x / 11.0f;
            const ImVec2 menu_offset(0, _menu.height());
            const ImVec2 char_size(width, _menu.height());

            for (auto& window : _windows) {
                Timer t;
                ImGui::SetNextWindowSize(window->size() * char_size, ImGuiCond_FirstUseEver);
                ImGui::SetNextWindowPos(window->pos() * char_size + ImVec2(0, _menu.height()), ImGuiCond_FirstUseEver);
                if (ImGui::Begin(window->name().c_str())) {
                    window->doit();
                }
                ImGui::End();
                window_timings[index].sample(t.stop() / 1000.0);
                index++;
            }
#if 0
            if (ImGui::Begin("Timings")) {
                for (int i = 0; i < _windows.size(); ++i) {
                    ImGui::Text("%s: %.2f ms", _windows[i]->name().c_str(), window_timings[i].get());
                }
            }
            ImGui::End();
#endif

            for (auto& dialog : _dialogs) {
                dialog->actualLaunch();
                if (ImGui::BeginPopupModal(dialog->name().c_str(), 0, ImGuiWindowFlags_AlwaysAutoResize)) {
                    dialog->doit();
                    ImGui::EndPopup();
                }
            }

            for (auto& event : _events) {
                event();
            }
            // Application can intercept quit by calling wantsToQuit()
            // in event(). If this is not done app will quit here.
            if (_wantsToQuit) {
                _quit = true;
            }
        } catch (std::exception& e) {
            std::cout << "Error: " << e.what() << std::endl;
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
        ImGui::StyleColorsClassic();
    }
}

ImGuiMenu& ImGuiApp::getMenu() {
    return _menu;
}

ImGuiSettings& ImGuiApp::getSettings() {
    return _settings;
}

void ImGuiApp::setTitle(const std::string& title) {
    glfwSetWindowTitle(_glfw_window, title.c_str());
}

bool ImGuiApp::wantsToQuit() {
    if (_wantsToQuit) {
        _wantsToQuit = false;
        return true;
    }
    return false;
}

void ImGuiApp::quit() {
    _quit = true;
}