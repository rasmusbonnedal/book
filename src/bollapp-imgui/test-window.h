#pragma once
#include "imgui-window.h"

class TestWindow : public ImGuiWindowBase {
   public:
    TestWindow() : ImGuiWindowBase("Test Window"), _f(0.0f), _counter(0) {}

    void doit() final;

   private:
    float _f;
    int _counter;
};
