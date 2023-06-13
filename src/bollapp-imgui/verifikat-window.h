#pragma once

#include "file-handler.h"
#include "imgui-window.h"

class VerifikatWindow : public ImGuiWindowBase {
   public:
    VerifikatWindow(FileHandler& file_handler);
    void doit() final;

   private:
    FileHandler& _file_handler;
    int _selected_row;
    std::vector<int> _verifikat_index;
};
