#pragma once

#include <memory>

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
};
