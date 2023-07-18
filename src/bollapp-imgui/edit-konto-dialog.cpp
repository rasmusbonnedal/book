#include "edit-konto-dialog.h"

#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

template<typename T>
std::string find(const std::vector<std::pair<T, std::string>>& vec, const T key) {
    for (const auto& [k, v] : vec) {
        if (k == key) {
            return v;
        }
    }
    return "";
}

template<typename T>
bool comboBox(const std::string& label, const std::vector<std::pair<T, std::string>>& combo, T& value) {
    bool updated = false;
    if (ImGui::BeginCombo(label.c_str(), find(combo, value).c_str())) {
        for (const auto& [k, v] : combo) {
            if (ImGui::Selectable(v.c_str(), k == value)) {
                value = k;
                updated = true;
            }
            if (k == value) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }
    return updated;
}

EditKontoDialog::EditKontoDialog(FileHandler& file_handler) : ImGuiDialog(u8"Ändra konto"), m_file_handler(file_handler), m_konto(0, "", 0) {}

void EditKontoDialog::doit() {
    int unid = m_konto.getUnid();
    bool konto_valid = true;
    if (m_new_konto) {
        if (m_konto.getUnid() == 0 || m_file_handler.getDoc().getKontoPlan().count(m_konto.getUnid()) > 0) {
            konto_valid = false;
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
        }
        ImGui::InputInt("Nummer", &unid, -1, -1);
        if (!konto_valid) {
            ImGui::PopStyleColor();        
        }
        if (unid != m_konto.getUnid()) {
            m_konto = BollDoc::Konto(unid, m_konto.getText(), m_konto.getTyp(), m_konto.getNormalt(), m_konto.getTagg());
        }    
    } else {
        ImGui::InputInt("Nummer", &unid, -1, -1, ImGuiInputTextFlags_ReadOnly);
    }
    bool namn_valid = !m_konto.getText().empty();
    if (!namn_valid) {
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
    }
    ImGui::InputText("Namn", &m_konto.getText());
    if (!namn_valid) {
        ImGui::PopStyleColor();
    }
    ImGui::InputText("Tagg", &m_konto.getTagg());
    const std::vector<std::pair<int, std::string>> typ_combo = {{1, "Balans"}, {3, "Resultat"}};
    bool typ_valid = m_konto.getTyp() == 1 || m_konto.getTyp() == 3;
    if (!typ_valid) {
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
    }
    comboBox("Typ", typ_combo, m_konto.getTyp());
    if (!typ_valid) {
        ImGui::PopStyleColor();
    }
    const std::vector<std::pair<std::string, std::string>> norm_combo = {{"+", "+"}, {"-", "-"}, {"", "None"}};
    comboBox("Normal", norm_combo, m_konto.getNormalt());

    ImGui::Separator();
    bool total_valid = konto_valid && namn_valid && typ_valid;
    if (!total_valid) {
        ImGui::BeginDisabled();
    }
    if (ImGui::Button("OK")) {
        m_file_handler.getDoc().addOrUpdateKonto(std::move(m_konto));
        ImGui::CloseCurrentPopup();
    }
    if (!total_valid) {
        ImGui::EndDisabled();
    }
    ImGui::SameLine();
    if (ImGui::Button("Cancel")) {
        ImGui::CloseCurrentPopup();
    }
}
