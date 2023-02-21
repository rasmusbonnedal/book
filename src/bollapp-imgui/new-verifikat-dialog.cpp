#include "new-verifikat-dialog.h"

#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

void NewVerifikatDialog::doit() {
    ImGui::InputInt("Resolution", &_resolution);
    ImGui::InputText("UV set name", &_uvsetname);
}
