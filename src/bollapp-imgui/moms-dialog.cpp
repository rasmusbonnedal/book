#include "moms-dialog.h"

#include <fstream>

#include <imgui.h>

#include "date.h"
#include "file-handler.h"
#include "moms.h"

namespace {
void imguiRightAlign(const char* text) {
    auto posX = (ImGui::GetCursorPosX() + ImGui::GetColumnWidth() - ImGui::CalcTextSize(text).x - ImGui::GetScrollX() -
                 0 * ImGui::GetStyle().ItemSpacing.x);
    if (posX > ImGui::GetCursorPosX()) {
        ImGui::SetCursorPosX(posX);
    }
}

// TODO!!! Import from kontomap json
const KontoMap konto_map{{05, {3011}}, {10, {2611}}, {42, {3014, 3690}}, {48, {2641}}};
const FieldToSkv field_to_skv = {{5, "ForsMomsEjAnnan"}, {10, "MomsUtgHog"}, {42, "ForsOvrigt"}, {48, "MomsIngAvdr"}, {49, "MomsBetala"}};
}  // namespace

MomsDialog::MomsDialog(FileHandler& file_handler)
    : ImGuiDialog("Momsredovisning"), m_file_handler(file_handler), m_datum(DATETYPE_JANUARY) {}

void MomsDialog::doit() {
    if (ImGui::BeginCombo(u8"Månad", dateTypeToString((DateType)m_datum))) {
        for (int n = DATETYPE_JANUARY; n < DATETYPE_COUNT; ++n) {
            bool is_selected = (n == m_datum);
            if (ImGui::Selectable(dateTypeToString((DateType)n), is_selected)) {
                m_datum = n;
            }
            if (is_selected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }

    Verifikat redovisning;
    const int month = m_datum + 1 - DATETYPE_JANUARY;
    FieldSaldo result;
    try {
        result = summarize_moms(m_file_handler.getDoc(), month, konto_map, redovisning);
    } catch (std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    ImGui::Separator();
    ImGui::TextUnformatted("Moms eSKD");
    if (ImGui::BeginTable("Moms eSKD", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_NoBordersInBody)) {
        ImGui::TableSetupColumn(u8"Fält");
        ImGui::TableSetupColumn("Beskrivning");
        ImGui::TableSetupColumn("Saldo");
        ImGui::TableSetupScrollFreeze(0, 1);
        ImGui::TableHeadersRow();

        for (const auto& [field, saldo] : result) {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("%d", field);
            ImGui::TableSetColumnIndex(1);
            ImGui::TextUnformatted(field_to_skv.find(field)->second.c_str());
            ImGui::TableSetColumnIndex(2);
            const std::string saldo_str = to_string(saldo);
            imguiRightAlign(saldo_str.c_str());
            ImGui::TextUnformatted(saldo_str.c_str());
        }
        ImGui::EndTable();
    }

    ImGui::Separator();
    ImGui::TextUnformatted("Momsredovisning");
    if (ImGui::BeginTable("Momsredovisning", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_NoBordersInBody)) {
        ImGui::TableSetupColumn("Konto");
        ImGui::TableSetupColumn("Beskrivning");
        ImGui::TableSetupColumn("Saldo");
        ImGui::TableSetupScrollFreeze(0, 1);
        ImGui::TableHeadersRow();

        for (const auto& [konto, saldo] : redovisning) {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("%d", konto);
            ImGui::TableSetColumnIndex(1);
            ImGui::TextUnformatted(m_file_handler.getDoc().getKonto(konto).getText().c_str());
            ImGui::TableSetColumnIndex(2);
            const std::string saldo_str = to_string(saldo);
            imguiRightAlign(saldo_str.c_str());
            ImGui::TextUnformatted(saldo_str.c_str());
        }

        ImGui::EndTable();
    }

    if (ImGui::Button("Create")) {
        auto& doc = m_file_handler.getDoc();
        int id = doc.getNextVerifikatId();
        int year = doc.getBokforingsar();
        Date date = lastDayOfMonth(Date(year, month, 1));
        BollDoc::Verifikat ver(id, "Momsredovisning " + std::string(dateTypeToString((DateType)m_datum)) + " " + std::to_string(year), date);
        for (const auto& [konto, saldo] : redovisning) {
            ver.addRad(BollDoc::Rad(now(), konto, saldo));        
        }

        try {
            std::string xml = gen_moms_eskd(doc, month, result, field_to_skv);
            std::string filename;
            FileDialogResult result = fileSaveDialog("xml", filename);
            if (result == FDR_OKAY) {
                std::ofstream ofs(filename, std::ios::out);
                if (ofs.is_open()) {
                    ofs << xml;
                    doc.addVerifikat(std::move(ver));
                    ImGui::CloseCurrentPopup();
                }
            }
        } catch (std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;        
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("Cancel")) {
        ImGui::CloseCurrentPopup();
    }
}

void MomsDialog::launch() {
    ImGuiDialog::launch();
}
