#include "konto-window.h"

#include <imgui.h>

#include "bolldoc.h"
#include "book-app.h"
#include "edit-konto-dialog.h"

KontoWindow::KontoWindow(FileHandler& file_handler, BookApp& book_app)
    : ImGuiWindowBase("Konton"), _file_handler(file_handler), _book_app(book_app) {}

void KontoWindow::doit() {
    if (ImGui::BeginTable("konton", 5,
                          ImGuiTableFlags_Resizable | ImGuiTableFlags_Sortable | ImGuiTableFlags_SortTristate | ImGuiTableFlags_Borders |
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

        int row = 0;

        for (const auto& [kontonr, konto] : kontoplan) {
            ImGui::TableNextRow();
            // Konto
            ImGui::TableSetColumnIndex(0);
            std::string s = std::to_string(kontonr);
            bool st = row == _selected_row;
            if (ImGui::Selectable(s.c_str(), &st, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowDoubleClick)) {
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
            row++;
        }

        ImGui::EndTable();
    }
}
