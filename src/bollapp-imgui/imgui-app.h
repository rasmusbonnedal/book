#pragma once

#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "imgui-menu.h"
#include "imgui-settings.h"

class ImGuiWindowBase;
class ImGuiDialog;
struct GLFWwindow;

class ImGuiApp {
   public:
    ImGuiApp(const std::string& name);
    ~ImGuiApp();
    void addWindow(std::shared_ptr<ImGuiWindowBase> window);
    void addDialog(std::shared_ptr<ImGuiDialog> dialog);
    void addEvent(std::function<void()> event);
    void run();
    bool wantsToQuit();

    void setStyle(bool dark);
    ImGuiMenu& getMenu();
    ImGuiSettings& getSettings();
    void setTitle(const std::string& title);
    void quit();

   private:
    bool _wantsToQuit;
    bool _quit;
    std::string _name;
    std::vector<std::shared_ptr<ImGuiWindowBase>> _windows;
    std::vector<std::shared_ptr<ImGuiDialog>> _dialogs;
    std::vector<std::function<void()>> _events;
    ImGuiMenu _menu;
    ImGuiSettings _settings;
    GLFWwindow* _glfw_window;
};
