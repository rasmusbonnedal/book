#include "konto-window.h"

#include <imgui.h>

#include "bolldoc.h"
#include "book-app.h"
#include "edit-konto-dialog.h"

namespace {
void sortKonton(const std::map<int, BollDoc::Konto>& kontoplan, std::vector<int>& kontoplan_index, ImGuiTableSortSpecs* sort_spec) {
    if (sort_spec->SpecsCount == 0) {
        return;
    }
    std::sort(kontoplan_index.begin(), kontoplan_index.end(), [sort_spec, kontoplan](int lhs, int rhs) {
        auto& spec = sort_spec->Specs[0];
        if (spec.SortDirection == ImGuiSortDirection_Descending) {
            std::swap(lhs, rhs);
        }
        if (spec.ColumnIndex == 0) {
            return lhs > rhs;
        }
        const auto& konto_a = kontoplan.find(lhs)->second;
        const auto& konto_b = kontoplan.find(rhs)->second;
        if (spec.ColumnIndex == 1) {
            return konto_a.getText().compare(konto_b.getText()) > 0;
        }
        if (spec.ColumnIndex == 2) {
            return konto_a.getTagg().compare(konto_b.getTagg()) > 0;
        }
        if (spec.ColumnIndex == 3) {
            return konto_a.getTyp() > konto_b.getTyp();
        }
        if (spec.ColumnIndex == 4) {
            return konto_a.getNormalt() > konto_b.getNormalt();
        }
        return lhs > rhs;
    });
}
}

KontoWindow::KontoWindow(FileHandler& file_handler, BookApp& book_app)
    : ImGuiWindowBase("Konton"), _file_handler(file_handler), _book_app(book_app) {}

void KontoWindow::doit() {
    if (ImGui::Button("Nytt konto")) {
        _book_app.editKontoDialog().launch();    
    }
    if (ImGui::BeginTable("konton", 5,
                          ImGuiTableFlags_Resizable | ImGuiTableFlags_Sortable | ImGuiTableFlags_Borders |
                              ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY | ImGuiTableFlags_NoBordersInBody)) {
        const auto& doc = _file_handler.getDoc();
        const auto& kontoplan = doc.getKontoPlan();
        ImGui::TableSetupColumn("Konto", ImGuiTableColumnFlags_DefaultSort);
        ImGui::TableSetupColumn("Beskrivning");
        ImGui::TableSetupColumn("Tagg");
        ImGui::TableSetupColumn("Typ");
        ImGui::TableSetupColumn("Normal");
        ImGui::TableSetupScrollFreeze(0, 1);
        ImGui::TableHeadersRow();

        bool should_sort = false;
        auto sort_spec = ImGui::TableGetSortSpecs();
        if (sort_spec->SpecsDirty) {
            sort_spec->SpecsDirty = false;
            should_sort = true;
        }

        if (_kontoplan_index.size() != kontoplan.size() || should_sort) {
            _kontoplan_index.clear();
            _kontoplan_index.reserve(kontoplan.size());
            for (const auto& [kontonr, _] : kontoplan) {
                _kontoplan_index.push_back(kontonr);
            }
            sortKonton(kontoplan, _kontoplan_index, sort_spec);
        }

        ImGuiListClipper clipper;
        clipper.Begin((int)_kontoplan_index.size());
        while (clipper.Step()) {
            for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; row++) {
                int kontonr = _kontoplan_index[row];
                const auto& konto = kontoplan.find(kontonr)->second;
                ImGui::TableNextRow();
                // Konto
                ImGui::TableSetColumnIndex(0);
                char buf[8];
                snprintf(buf, sizeof(buf), "%d", kontonr);
                bool st = row == _selected_row;
                if (ImGui::Selectable(buf, &st, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowDoubleClick)) {
                    _selected_row = st ? row : -1;
                    if (ImGui::IsMouseDoubleClicked(0)) {
                        _book_app.editKontoDialog().launch(kontonr);
                    } else {
                    }
                }

                // Beskrivning
                ImGui::TableSetColumnIndex(1);
                ImGui::TextUnformatted(konto.getText().c_str());
                // Tagg
                ImGui::TableSetColumnIndex(2);
                ImGui::TextUnformatted(konto.getTagg().c_str());
                // Typ
                ImGui::TableSetColumnIndex(3);
                ImGui::TextUnformatted((konto.getTyp() == 1) ? "Balans" : "Resultat");
                // Normal
                ImGui::TableSetColumnIndex(4);
                ImGui::TextUnformatted(konto.getNormalt().c_str());
            }
        }
        ImGui::EndTable();
    }
}
