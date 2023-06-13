#include "imgui-dialog.h"

#include <imgui.h>

void ImGuiDialog::launch() {
    _launch = true;
}

void ImGuiDialog::actualLaunch() {
    if (_launch) {
        ImGui::OpenPopup(_name.c_str());
        _launch = false;
    }
}