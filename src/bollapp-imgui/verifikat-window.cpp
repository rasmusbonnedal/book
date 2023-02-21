#include "verifikat-window.h"

#include "bolldoc.h"

#include <imgui.h>

#include <iostream>

VerifikatWindow::VerifikatWindow(FileHandler& file_handler)
    : ImGuiWindowBase("Verifikat"), _selected_row(-1),
      _file_handler(file_handler) {}

namespace {
void imguiRightAlign(const char* text) {
    auto posX = (ImGui::GetCursorPosX() + ImGui::GetColumnWidth() -
                 ImGui::CalcTextSize(text).x - ImGui::GetScrollX() -
                 0 * ImGui::GetStyle().ItemSpacing.x);
    if (posX > ImGui::GetCursorPosX()) {
        ImGui::SetCursorPosX(posX);
    }
}
} // namespace

void VerifikatWindow::doit() {
    if (ImGui::BeginTable("verifikat", 4,
                          ImGuiTableFlags_Resizable | ImGuiTableFlags_Sortable |
                              ImGuiTableFlags_SortTristate |
                              ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg |
                              ImGuiTableFlags_ScrollY |
                              ImGuiTableFlags_NoBordersInBody)) {
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
                // std::cout << "Sorting: " << spec.ColumnIndex << ", " <<
                // spec.SortDirection << std::endl;
            }
        }

        BollDoc& doc = _file_handler.getDoc();
        int row = 0;
        for (auto& verifikat: doc.getVerifikationer()) {
            ImGui::TableNextRow();
            // Id
            ImGui::TableSetColumnIndex(0);
            std::string s = std::to_string(verifikat.getUnid());
            bool st = row == _selected_row;
            if (ImGui::Selectable(s.c_str(), &st,
                                  ImGuiSelectableFlags_SpanAllColumns)) {
                _selected_row = st ? row : -1;
            }

            ImGui::TableSetColumnIndex(1);
            s = to_string(verifikat.getTransdatum());
            ImGui::TextUnformatted(s.c_str());

            ImGui::TableSetColumnIndex(2);
            ImGui::TextUnformatted(verifikat.getText().c_str());

            // Omslutning
            ImGui::TableSetColumnIndex(3);
            Pengar omslutning;
            if (verifikat.getOmslutning(omslutning)) {
                s = toString2(omslutning) + " kr";
                imguiRightAlign(s.c_str());
            } else {
                s = "- obalanserad -";
            }
            ImGui::TextUnformatted(s.c_str());            
            row++;
        }
        ImGui::EndTable();
    }
}
