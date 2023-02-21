#pragma once

#include <memory>

#include "file-handler.h"
#include "imgui-app.h"

class VerifikatWindow;
class NewVerifikatDialog;

class BookApp {
public:
    BookApp();
    ~BookApp();

    void run();

private:
    void event();

    std::shared_ptr<VerifikatWindow> _verifikat_window;
    std::shared_ptr<NewVerifikatDialog> _new_verifikat_dialog;
    ImGuiApp _app;
    FileHandler _file_handler;
};
