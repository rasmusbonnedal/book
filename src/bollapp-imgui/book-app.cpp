#include "book-app.h"

#include <imgui.h>
#include <GLFW/glfw3.h>

#include "new-verifikat-dialog.h"
#include "verifikat-window.h"
#include "imgui-menu.h"

BookApp::BookApp() : _app("Bokföring") {
    _verifikat_window = std::make_shared<VerifikatWindow>(_file_handler);
    _app.addWindow(_verifikat_window);
    _new_verifikat_dialog = std::make_shared<NewVerifikatDialog>();
    _app.addDialog(_new_verifikat_dialog);
    _app.addEvent(std::bind(&BookApp::event, this));
    _app.setStyle(false);
    ImGuiMenuHeader file_menu("File");
    std::function<void()> f = std::bind(&FileHandler::open, &_file_handler);
    file_menu.addItem(ImGuiMenuItem("Open", "CTRL-O", f));
    _app.getMenu().addHeader(std::move(file_menu));
}

BookApp::~BookApp() {}

void BookApp::run() {
    _app.run();
}

void BookApp::event() {
    ImGuiIO& io = ImGui::GetIO();
    if (io.KeyCtrl && io.KeysDown[GLFW_KEY_N]) {
        _new_verifikat_dialog->launch();
    }
}
