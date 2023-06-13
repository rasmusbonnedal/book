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
    ImGui::InputInt("Nummer", &unid, -1, -1, ImGuiInputTextFlags_ReadOnly);
    ImGui::InputText("Namn", &m_konto.getText());
    ImGui::InputText("Tagg", &m_konto.getTagg());
    const std::vector<std::pair<int, std::string>> typ_combo = {{1, "Balans"}, {3, "Resultat"}};
    comboBox("Typ", typ_combo, m_konto.getTyp());
    const std::vector<std::pair<std::string, std::string>> norm_combo = {{"+", "+"}, {"-", "-"}, {"", "None"}};
    comboBox("Normal", norm_combo, m_konto.getNormalt());

    ImGui::Separator();
    if (ImGui::Button("OK")) {
        m_file_handler.getDoc().addOrUpdateKonto(std::move(m_konto));
        ImGui::CloseCurrentPopup();
    }
    ImGui::SameLine();
    if (ImGui::Button("Cancel")) {
        ImGui::CloseCurrentPopup();
    }
}
