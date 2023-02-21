#pragma once

#include "imgui-window.h"

class VerifikatWindow : public ImGuiWindowBase {
   public:
    VerifikatWindow();
    void doit() final;

   private:
   int _selected_row;
};
