#pragma once

#include <functional>
#include <memory>
#include <string>
#include <vector>

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

    void setStyle(bool dark);

   private:
    std::string _name;
    std::vector<std::shared_ptr<ImGuiWindowBase>> _windows;
    std::vector<std::shared_ptr<ImGuiDialog>> _dialogs;
    std::vector<std::function<void()>> _events;
    GLFWwindow* _glfw_window;
};
