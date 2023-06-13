#include "verifikat-window.h"

#include <imgui.h>

#include <iostream>

#include "bolldoc.h"

namespace {
void sortVerifikat(const std::vector<BollDoc::Verifikat>& verifikat, std::vector<int>& verifikat_index, ImGuiTableSortSpecs* sort_spec) {
    if (sort_spec->SpecsCount == 0) {
        return;
    }
    std::sort(verifikat_index.begin(), verifikat_index.end(), [sort_spec, verifikat](int lhs, int rhs) {
        auto& spec = sort_spec->Specs[0];
        if (spec.SortDirection == ImGuiSortDirection_Descending) {
            std::swap(lhs, rhs);
        }
        if (spec.ColumnIndex == 0) {
            return lhs > rhs;
        }
        const auto& v1 = verifikat[lhs];
        const auto& v2 = verifikat[rhs];
        if (spec.ColumnIndex == 1) {
            const auto& date1 = v1.getTransdatum();
            const auto& date2 = v2.getTransdatum();
            if (date1 == date2) {
                return lhs > rhs;
            }
            return date2 < date1;
        }
        return lhs > rhs;
    });
}
}  // namespace

VerifikatWindow::VerifikatWindow(FileHandler& file_handler)
    : ImGuiWindowBase("Verifikat"), _selected_row(-1), _file_handler(file_handler) {}

namespace {
void imguiRightAlign(const char* text) {
    auto posX = (ImGui::GetCursorPosX() + ImGui::GetColumnWidth() - ImGui::CalcTextSize(text).x - ImGui::GetScrollX() -
                 0 * ImGui::GetStyle().ItemSpacing.x);
    if (posX > ImGui::GetCursorPosX()) {
        ImGui::SetCursorPosX(posX);
    }
}
}  // namespace

void VerifikatWindow::doit() {
    if (ImGui::BeginTable("verifikat", 4,
                          ImGuiTableFlags_Resizable | ImGuiTableFlags_Sortable | ImGuiTableFlags_Borders |
                              ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY | ImGuiTableFlags_NoBordersInBody)) {
        ImGui::TableSetupColumn("Nr");
        ImGui::TableSetupColumn("Datum", ImGuiTableColumnFlags_DefaultSort);
        ImGui::TableSetupColumn("Beskrivning", ImGuiTableColumnFlags_NoSort);
        ImGui::TableSetupColumn("Omslutning", ImGuiTableColumnFlags_NoSort);
        ImGui::TableSetupScrollFreeze(0, 1);
        ImGui::TableHeadersRow();

        bool should_sort = false;
        auto sort_spec = ImGui::TableGetSortSpecs();
        if (sort_spec->SpecsDirty) {
            sort_spec->SpecsDirty = false;
            should_sort = true;
        }

        BollDoc& doc = _file_handler.getDoc();
        const auto& verifikationer = doc.getVerifikationer();
        if (_verifikat_index.size() != verifikationer.size() || should_sort) {
            _verifikat_index.clear();
            _verifikat_index.reserve(verifikationer.size());
            for (size_t i = 0; i < verifikationer.size(); ++i) {
                _verifikat_index.push_back((int)i);
            }
            sortVerifikat(verifikationer, _verifikat_index, sort_spec);
        }

        char display_buf[256];
        ImGuiListClipper clipper;
        clipper.Begin((int)verifikationer.size());
        while (clipper.Step()) {
            for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; row++) {
                const auto& verifikat = verifikationer[_verifikat_index[row]];
                ImGui::TableNextRow();
                // Id
                ImGui::TableSetColumnIndex(0);
                snprintf(display_buf, sizeof(display_buf), "%d", verifikat.getUnid());
                bool st = row == _selected_row;
                if (ImGui::Selectable(display_buf, &st, ImGuiSelectableFlags_SpanAllColumns)) {
                    _selected_row = st ? row : -1;
                }

                ImGui::TableSetColumnIndex(1);
                to_string(verifikat.getTransdatum(), display_buf);
                ImGui::TextUnformatted(display_buf);

                ImGui::TableSetColumnIndex(2);
                ImGui::TextUnformatted(verifikat.getText().c_str());

                // Omslutning
                ImGui::TableSetColumnIndex(3);
                Pengar omslutning;
                if (verifikat.getOmslutning(omslutning)) {
                    to_string(omslutning, display_buf);
                    imguiRightAlign(display_buf);
                    ImGui::TextUnformatted(display_buf);
                } else {
                    ImGui::TextUnformatted("- obalanserad -");
                }
            }
        }
        ImGui::EndTable();
    }
}
