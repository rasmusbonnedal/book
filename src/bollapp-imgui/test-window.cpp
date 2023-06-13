#include "test-window.h"

#include <imgui.h>

void TestWindow::doit() {
    ImGui::Text("This is some useful text.");  // Display some text (you can use a format strings too)

    ImGui::SliderFloat("float", &_f, 0.0f, 1.0f);  // Edit 1 float using a slider from 0.0f to 1.0f

    if (ImGui::Button("Button"))  // Buttons return true when clicked (most widgets return true when edited/activated)
        _counter++;
    ImGui::SameLine();
    ImGui::Text("counter = %d", _counter);

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
}
