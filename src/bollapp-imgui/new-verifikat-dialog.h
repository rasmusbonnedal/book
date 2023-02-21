#pragma once

#include "imgui-dialog.h"

class NewVerifikatDialog : public ImGuiDialog {
   public:
    NewVerifikatDialog() : ImGuiDialog("Nytt verifikat") {}
    void doit() final;
   private:
    int _resolution;
    std::string _uvsetname;
};
