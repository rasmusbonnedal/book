#include "imgui-dialog.h"

#include <imgui.h>

void ImGuiDialog::launch() {
    ImGui::OpenPopup(_name.c_str());
}
