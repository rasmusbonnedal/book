#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

#include <iostream>

#include "imgui-app.h"
#include "imgui-window.h"
#include "imgui_combo_autoselect.h"

class ComboWindow : public ImGuiWindowBase {
   public:
    ComboWindow()
        : ImGuiWindowBase("Test Combo"),
          _items({{"1900 Konto", "1901 Bankkonto", "1991 Sparkonto", "1992 Sparkonto 2", "2040 Skatteskuld"}})
           , _data(_items) {
        f1 = 1.0f;
        f2 = 0.0f;
    }
    void doit() {
        if (GImGui->CurrentWindow->Appearing) {
            ImGui::SetKeyboardFocusHere(0);
        }
        if (ImGui::InputText("text", &_buf, ImGuiInputTextFlags_EnterReturnsTrue)) {
            ImGui::SetKeyboardFocusHere(0);
        }

        ImGui::SetNextItemWidth(120);
        if (ImGui::ComboAutoSelect("##combo1", _data, _items)) {
            ImGui::SetKeyboardFocusHere(0);
        }
        ImGui::SameLine();
        if (ImGui::InputFloat("##saldo1", &f2, 0, 0, "%.2f", ImGuiInputTextFlags_EnterReturnsTrue)) {
            ImGui::SetKeyboardFocusHere(0);
        }
        ImGui::Text("Data: %s, Index: %d", _data.input, _data.index);
        ImGui::Button("Hello");
        ImGui::SameLine();
        ImGui::Button("Goodbye");
    }

   private:
    ImGui::ComboAutoSelectData _data;
    std::vector<std::string> _items;
    float f1;
    float f2;
    std::string _buf;
};

int main(int, char**) {
    try {
        ImGuiApp app("Test Combo");
        auto win = std::make_shared<ComboWindow>();
        app.addWindow(win);
        app.run();
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
    return 0;
}
