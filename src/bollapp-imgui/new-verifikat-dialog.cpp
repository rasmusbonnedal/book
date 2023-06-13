#include "new-verifikat-dialog.h"

#include <imgui.h>
#include <imgui_internal.h>
#include <misc/cpp/imgui_stdlib.h>

#include "bolldoc.h"

#include "file-handler.h"

NewVerifikatDialog::NewVerifikatDialog(FileHandler& file_handler) : ImGuiDialog("Nytt verifikat"), m_file_handler(file_handler) {}

void NewVerifikatDialog::launchVer() {
    m_date_ok = true;
    int unid = m_file_handler.getDoc().getNextVerifikatId();
    Date date;
    if (unid > 0) {
        date = m_file_handler.getDoc().getVerifikat(unid - 1).getTransdatum();
    } else {
        date = Date(now().getYear(), 1, 1);
    }
    m_verifikat = std::make_unique<BollDoc::Verifikat>(unid, "", date);
    m_date = to_string(m_verifikat->getTransdatum());
    m_konton.clear();
    for (const auto& [id, konto] : m_file_handler.getDoc().getKontoPlan()) {
        m_konton.push_back(std::to_string(id) + " " + konto.getText());
        m_konton_id.push_back(id);
    }
    m_konto_rad_data.clear();
    m_pengar_rad.clear();
    m_konto_rad_data.push_back(m_konton);
    m_pengar_rad.push_back(0.0f);
    ImGuiDialog::launch();
}

void NewVerifikatDialog::doit() {
    if (GImGui->CurrentWindow->Appearing) {
        ImGui::SetKeyboardFocusHere();
    }
    if (ImGui::InputText("Text", &m_verifikat->getText(), ImGuiInputTextFlags_EnterReturnsTrue)) {
        ImGui::SetKeyboardFocusHere();
    }
    if (ImGui::InputText("Datum", &m_date, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CharsDecimal)) {
        ImGui::SetKeyboardFocusHere();
    }
    if (ImGui::IsItemDeactivatedAfterEdit()) {
        if (auto d = parseDateNothrow(m_date)) {
            m_verifikat->setTransdatum(*d);
            m_date_ok = true;
        } else {
            m_date_ok = false;
        }
    }
    
    for (size_t i = 0; i < m_konto_rad_data.size(); ++i) {
        std::string id = "##kontocombo" + std::to_string(i);
        if (ImGui::ComboAutoSelect(id.c_str(), m_konto_rad_data[i], m_konton, 0)) {
            ImGui::SetKeyboardFocusHere();
            bool is_last_item = i + 1 == m_konto_rad_data.size();
            bool is_zero = m_pengar_rad[i] == 0.0f;
            bool is_konto_set = m_konto_rad_data[i].index > 0;
            if (is_last_item && is_konto_set && is_zero) {
                int balans = 0;
                for (float v : m_pengar_rad) {
                    balans += (int)(v * 100.0f);
                }
                if (balans != 0) {
                    m_pengar_rad[i] = -balans * 0.01f;
                }
            }
        }
        ImGui::SameLine();
        id = "##pengarbox" + std::to_string(i);
        ImGui::InputFloat(id.c_str(), &m_pengar_rad[i], 0.0f, 0.0f, "%.2f kr", 0);
        if (ImGui::IsItemDeactivated()) {
            ImGui::SetKeyboardFocusHere();
        }
    }
    int balans = 0;
    for (float v : m_pengar_rad) {
        balans += (int)(v * 100.0f);
    }
    if (balans == 0) {
        ImGui::Text("Raderna balanserar!");
    } else {
        ImGui::Text("Balans: %.2f kr", balans * 0.01f);
    }
    if (m_konto_rad_data.back().index >= 0 && m_pengar_rad.back() != 0.0f) {
        m_konto_rad_data.push_back(m_konton);
        m_pengar_rad.push_back(0.0f);
    }

    bool rader_ok = true;

    bool in_active_rows = true;
    for (size_t i = 0; i < m_konto_rad_data.size(); ++i) {
        int konto = m_konto_rad_data[i].index;
        int pengar = (int)(m_pengar_rad[i] * 100.0f);
        bool rad_ok = konto >= 0 && pengar != 0;
        bool rad_empty = konto == -1 && pengar == 0;

        if (in_active_rows) {
            if (rad_ok) {
                continue;
            } else if (rad_empty && i >= 2) {
                in_active_rows = false;
                continue;
            } else {
                rader_ok = false;
                break;
            }
        } else {
            if (rad_empty) {
                continue;
            } else {
                rader_ok = false;
                break;
            }
        }
    }

    ImGui::Separator();
    ImGui::BeginDisabled(!m_date_ok || m_verifikat->getText().empty() || balans != 0 || !rader_ok);
    if (ImGui::Button("Add")) {
        for (size_t i = 0; i < m_konto_rad_data.size(); ++i) {
            int konto_idx = m_konto_rad_data[i].index;
            if (konto_idx >= 0) {
                int konto = m_konton_id[konto_idx];
                int pengar = (int)(m_pengar_rad[i] * 100.0f);
                m_verifikat->addRad(BollDoc::Rad(now(), konto, Pengar(pengar)));
            }
        }
        m_file_handler.getDoc().addVerifikat(std::move(*m_verifikat));
        m_verifikat.release();
        ImGui::CloseCurrentPopup();
    }
    ImGui::EndDisabled();
    ImGui::SameLine();
    if (ImGui::Button("Cancel")) {
        m_verifikat.release();
        ImGui::CloseCurrentPopup();
    }
}
