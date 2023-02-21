#include "verifikat-window.h"

#include <imgui.h>

#include <iostream>

VerifikatWindow::VerifikatWindow() : ImGuiWindowBase("Verifikat"), _selected_row(-1) {}

void VerifikatWindow::doit() {
    if (ImGui::BeginTable("verifikat", 4,
                          ImGuiTableFlags_Resizable | ImGuiTableFlags_Sortable | ImGuiTableFlags_SortTristate | ImGuiTableFlags_Borders |
                              ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY | ImGuiTableFlags_NoBordersInBody)) {
        ImGui::TableSetupColumn("Nr");
        ImGui::TableSetupColumn("Datum", ImGuiTableColumnFlags_DefaultSort);
        ImGui::TableSetupColumn("Beskrivning");
        ImGui::TableSetupColumn("Omslutning", ImGuiTableColumnFlags_NoSort);
        ImGui::TableSetupScrollFreeze(0, 1);
        ImGui::TableHeadersRow();
        auto sort_spec = ImGui::TableGetSortSpecs();
        if (sort_spec->SpecsDirty) {
            sort_spec->SpecsDirty = false;
            if (sort_spec->SpecsCount == 1) {
                auto& spec = sort_spec->Specs[0];
                // std::cout << "Sorting: " << spec.ColumnIndex << ", " << spec.SortDirection << std::endl;
            }
        }

        for (int row = 0; row < 100; row++) {
            ImGui::TableNextRow();
            // Id
            ImGui::TableSetColumnIndex(0);
            std::string s = std::to_string(row);
            bool st = row == _selected_row;
            if (ImGui::Selectable(s.c_str(), &st, ImGuiSelectableFlags_SpanAllColumns)) {
                _selected_row = st ? row : -1;
            }

            // Datum
            ImGui::TableSetColumnIndex(1);
            ImGui::TextUnformatted("2022-12-31");

            // Beskrivning
            ImGui::TableSetColumnIndex(2);
            ImGui::TextUnformatted("Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.");

            // Omslutning
            ImGui::TableSetColumnIndex(3);
            ImGui::TextUnformatted("0,00 kr");
        }
        ImGui::EndTable();
    }
}
