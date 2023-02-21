#include "color.h"

#include <imgui.h>

void imguiAdjustSrgb() {
    ImDrawData* draw_data = ImGui::GetDrawData();
    for (int n = 0; n < draw_data->CmdListsCount; n++) {
        const ImDrawList* cmd_list = draw_data->CmdLists[n];
        const auto& vtx_buffer = cmd_list->VtxBuffer;
        for (int v = 0; v < vtx_buffer.Size; ++v) {
            auto& vtx = vtx_buffer.Data[v];
            vtx.col = srgbToLinearColor(vtx.col);
        }
    }
}

SrgbTable::SrgbTable() {
    for (int v = 0; v <= 255; ++v) {
        m_data[v] = (unsigned char)(srgbToLinearFloat(v / 255.0f) * 255.0f);
    }
}

const SrgbTable srgbTable;
