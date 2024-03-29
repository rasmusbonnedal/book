#pragma once

#include <string>
#include <vector>

#include "imgui.h"
#include "imgui_internal.h"

namespace ImGui {
struct ComboAutoSelectData {
    int index = -1;
    int cancel_index = -1;
    char input[128] = {};

    void setIndex(const std::vector<std::string>& items, int selected_index) {
        if (selected_index > -1 && selected_index < (int)items.size()) {
            strncpy(input, items[selected_index].c_str(), sizeof(input) - 1);
            index = selected_index;
        }
    }

    ComboAutoSelectData(const std::vector<std::string>& items, int selected_index = -1) {
        setIndex(items, selected_index);
    }
};

bool ComboAutoSelect(const char* label, ComboAutoSelectData& data, const std::vector<std::string>& items, ImGuiComboFlags flags = 0);

}  // namespace ImGui