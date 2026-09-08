#include "new-verifikat-dialog.h"

#include <imgui.h>
#include <imgui_internal.h>
#include <misc/cpp/imgui_stdlib.h>

#include "bolldoc.h"
#include "book-app.h"
#include "file-handler.h"
#include "saldo-window.h"

#ifdef WIN32
#include <windows.h>
#include <shellapi.h>
#endif

namespace {
void openDocument(const std::filesystem::path& file) {
#ifdef WIN32
    ShellExecute(NULL, "open", file.string().c_str(), NULL, NULL, SW_SHOWDEFAULT);
#endif
}

bool InputSaldo(const char* label, Pengar* pengar) {
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems) return false;

    ImGuiContext& g = *GImGui;
    ImGuiStyle& style = g.Style;

    char buf[64];
    to_string(*pengar, buf);

    // We call MarkItemEdited() ourselves by comparing the actual data rather than the string.
    ImGui::PushItemFlag(ImGuiItemFlags_NoMarkEdited, true);

    bool value_changed = false;
    bool parsed = true;
    if (ImGui::InputText(label, buf, IM_ARRAYSIZE(buf), ImGuiInputTextFlags_CharsDecimal | ImGuiInputTextFlags_AutoSelectAll)) {
        Pengar old_pengar = *pengar;
        if (!parsePengarNothrow(buf, *pengar)) {
            parsed = false;
            *pengar = old_pengar;
        } else {
            value_changed = !(*pengar == old_pengar);
        }
    }
    if (value_changed) {
        ImGui::MarkItemEdited(g.LastItemData.ID);
    }
    ImGui::PopItemFlag();
    return value_changed && parsed;
}
}  // namespace

NewVerifikatDialog::NewVerifikatDialog(FileHandler& file_handler, BookApp& app) : ImGuiDialog("Nytt verifikat"), m_file_handler(file_handler), m_app(app) {}

void NewVerifikatDialog::launchVer() {
    launchNew(false);
}

void NewVerifikatDialog::launchBokforingsorder() {
    launchNew(true);
}

void NewVerifikatDialog::launchNew(bool bokforingsorder) {
    m_date_ok = true;
    m_dialog_mode = NEW;
    setName(bokforingsorder ? "Ny bokföringsorder" : "Nytt verifikat");
    int unid = m_file_handler.getDoc().getNextVerifikatId();
    Date date;
    if (unid > 0) {
        date = m_file_handler.getDoc().getVerifikat(unid - 1).getTransdatum();
    } else {
        date = Date(now().getYear(), 1, 1);
    }
    m_verifikat = std::make_unique<BollDoc::Verifikat>(unid, "", date, bokforingsorder);
    m_date = to_string(m_verifikat->getTransdatum());
    m_konton.clear();
    m_konton_id.clear();
    for (const auto& [id, konto] : m_file_handler.getDoc().getKontoPlan()) {
        m_konton.push_back(std::to_string(id) + " " + konto.getText());
        m_konton_id.push_back(id);
    }
    m_konto_rad_data.clear();
    m_pengar_rad.clear();
    m_bokdatum_rad.clear();
    m_struken_rad.clear();
    // If an account is locked in the Saldo window, prefill the first line
    // with it so verifikats from an account statement get it right away.
    int locked_konto = m_app.saldoWindow().getLockedKonto();
    int locked_index = -1;
    for (size_t i = 0; i < m_konton_id.size(); ++i) {
        if (m_konton_id[i] == locked_konto) {
            locked_index = (int)i;
            break;
        }
    }
    m_konto_rad_data.emplace_back(m_konton, locked_index);
    m_pengar_rad.push_back(0);
    m_bokdatum_rad.push_back(now());
    m_struken_rad.push_back(std::nullopt);
    m_kvitton.clear();
    m_attached_kvitton.clear();
    m_can_attach_kvitto = m_file_handler.canAttachKvitto();
    ImGuiDialog::launch();
}

void NewVerifikatDialog::launchEdit(const BollDoc::Verifikat& verifikat) {
    m_date_ok = true;
    m_dialog_mode = EDIT;
    setName(verifikat.isBokforingsorder() ? "Editera bokföringsorder" : "Editera verifikat");
    m_verifikat = std::make_unique<BollDoc::Verifikat>(
        verifikat.getUnid(), verifikat.getText(), verifikat.getTransdatum(),
        verifikat.isBokforingsorder());
    m_date = to_string(m_verifikat->getTransdatum());
    m_konton.clear();
    m_konton_id.clear();
    for (const auto& [id, konto] : m_file_handler.getDoc().getKontoPlan()) {
        m_konton.push_back(std::to_string(id) + " " + konto.getText());
        m_konton_id.push_back(id);
    }
    m_konto_rad_data.clear();
    m_pengar_rad.clear();
    m_bokdatum_rad.clear();
    m_struken_rad.clear();

    for (const auto& rad : verifikat.getRader()) {
        int konto_index = -1;
        // TODO: Need lookupmap
        for (size_t i = 0; i < m_konton_id.size(); ++i) {
            if (m_konton_id[i] == rad.getKonto()) {
                konto_index = (int)i;
                break;
            }
        }
        m_konto_rad_data.emplace_back(m_konton, konto_index);
        m_pengar_rad.push_back(rad.getPengar());
        m_bokdatum_rad.push_back(rad.getBokdatum());
        m_struken_rad.push_back(rad.getStruken());
    }
    m_konto_rad_data.push_back(m_konton);
    m_pengar_rad.push_back(0);
    m_bokdatum_rad.push_back(now());
    m_struken_rad.push_back(std::nullopt);

    m_kvitton = m_file_handler.getKvitton(m_verifikat->getUnid());
    m_attached_kvitton.clear();
    m_can_attach_kvitto = m_file_handler.canAttachKvitto();
    ImGuiDialog::launch();
}

void NewVerifikatDialog::doit() {
    if (m_verifikat->isBokforingsorder()) {
        ImGui::TextColored(ImVec4(1.0f, 191.0f / 255.0f, 0.0f, 1.0f),
                           "Bokföringsorder (preliminärt verifikat)");
        ImGui::Separator();
    }
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
        m_date_ok = false;
        if (auto d = parseDateNothrow(m_date)) {
            if (d->getYear() == m_file_handler.getDoc().getBokforingsar()) {
                m_verifikat->setTransdatum(*d);
                m_date_ok = true;
            }
        }
    }

    Pengar balans = 0;
    for (size_t i = 0; i < m_pengar_rad.size(); ++i) {
        if (m_struken_rad[i].has_value()) {
            continue;
        }
        balans += m_pengar_rad[i];
    }
    int row_to_delete = -1;
    for (size_t i = 0; i < m_konto_rad_data.size(); ++i) {
        const bool struken = m_struken_rad[i].has_value();
        const bool editable = m_verifikat->isBokforingsorder() ||
                              m_bokdatum_rad[i] == now();
        const ImVec2 row_start = ImGui::GetCursorScreenPos();
        if (struken || !editable) {
            // A struck row is retained for audit purposes and must not be
            // changed into a different transaction. Ordinary verifikat rows
            // entered before today have the same protection, but can still
            // be struck. Bokföringsorder remain editable.
            ImGui::BeginDisabled();
        }
        std::string id = "##kontocombo" + std::to_string(i);
        if (ImGui::ComboAutoSelect(id.c_str(), m_konto_rad_data[i], m_konton, 0)) {
            ImGui::SetKeyboardFocusHere();
            bool is_last_item = i + 1 == m_konto_rad_data.size();
            bool is_zero = m_pengar_rad[i] == 0;
            bool is_konto_set = m_konto_rad_data[i].index > 0;
            if (is_last_item && is_konto_set && is_zero) {
                if (balans != 0) {
                    m_pengar_rad[i] = -balans;
                }
            }
        }
        bool transaction_hovered = (struken || !editable) &&
                                  ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled);
        ImGui::SameLine();
        id = "##pengarbox" + std::to_string(i);
        InputSaldo(id.c_str(), &m_pengar_rad[i]);
        bool jump_to_next = ImGui::IsItemDeactivated();
        transaction_hovered = transaction_hovered ||
                              ((struken || !editable) &&
                               ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled));
        const ImVec2 transaction_end = ImGui::GetItemRectMax();
        if (struken || !editable) {
            ImGui::EndDisabled();
        }
        if (m_dialog_mode == EDIT) {
            if (!struken) {
                ImGui::SameLine();
                ImGui::PushID(static_cast<int>(i));
                if (ImGui::Button("X")) {
                    if (m_verifikat->isBokforingsorder()) {
                        row_to_delete = static_cast<int>(i);
                    } else {
                        m_struken_rad[i] = now();
                    }
                }
                ImGui::PopID();
            }
        }
        if (struken) {
            const float y = row_start.y + ImGui::GetFrameHeight() * 0.5f;
            ImGui::GetWindowDrawList()->AddLine(
                ImVec2(row_start.x, y), ImVec2(transaction_end.x, y),
                ImGui::GetColorU32(ImGuiCol_Text), 2.0f);
            if (transaction_hovered) {
                ImGui::SetTooltip("Struken %s", to_string(*m_struken_rad[i]).c_str());
            }
        } else if (!editable && transaction_hovered) {
            ImGui::SetTooltip("Raden kan bara ändras samma dag som den bokfördes (%s). Du kan fortfarande stryka den.",
                              to_string(m_bokdatum_rad[i]).c_str());
        }
        if (jump_to_next) {
            ImGui::SetKeyboardFocusHere();
        }
    }
    if (row_to_delete >= 0) {
        auto row = static_cast<size_t>(row_to_delete);
        m_konto_rad_data.erase(m_konto_rad_data.begin() + row);
        m_pengar_rad.erase(m_pengar_rad.begin() + row);
        m_bokdatum_rad.erase(m_bokdatum_rad.begin() + row);
        m_struken_rad.erase(m_struken_rad.begin() + row);
        balans = 0;
        for (size_t i = 0; i < m_pengar_rad.size(); ++i) {
            if (!m_struken_rad[i].has_value()) {
                balans += m_pengar_rad[i];
            }
        }
    }
    if (balans == 0) {
        ImGui::Text("Raderna balanserar!");
    } else {
        ImGui::Text("Balans: %s", to_string(balans).c_str());
    }
    if (m_konto_rad_data.back().index >= 0 && m_pengar_rad.back() != 0) {
        m_konto_rad_data.push_back(m_konton);
        m_pengar_rad.push_back(0);
        m_bokdatum_rad.push_back(now());
        m_struken_rad.push_back(std::nullopt);
    }

    bool rader_ok = true;

    bool in_active_rows = true;
    for (size_t i = 0; i < m_konto_rad_data.size(); ++i) {
        // Skip struck rows
        if (m_struken_rad[i].has_value()) {
            continue;
        }
        int konto = m_konto_rad_data[i].index;
        bool rad_ok = konto >= 0 && m_pengar_rad[i] != 0;
        // If on the import verifikat, rows with 0 is ok
        if (m_verifikat->getUnid() == 0) {
            rad_ok = konto >= 0;
        }
        bool rad_empty = m_pengar_rad[i] == 0;

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

    // If an account is locked in the Saldo window, at least one row must
    // use it.
    bool locked_ok = true;
    if (m_dialog_mode == NEW) {
        int locked_konto = m_app.saldoWindow().getLockedKonto();
        if (locked_konto >= 0) {
            locked_ok = false;
            for (size_t i = 0; i < m_konto_rad_data.size(); ++i) {
                int idx = m_konto_rad_data[i].index;
                if (idx >= 0 && m_konton_id[idx] == locked_konto) {
                    locked_ok = true;
                    break;
                }
            }
        }
    }
    ImGui::Separator();
    ImGui::Text("Kvitton:");
    for (const auto& kvitto : m_kvitton) {
        if (ImGui::Button(kvitto.filename().string().c_str())) {
            openDocument(kvitto);
        }
    }
    ImGui::Text("To attach:");
    int i = 0;
    int to_erase = -1;
    for (const auto& kvitto : m_attached_kvitton) {
        if (ImGui::Button(kvitto.u8string().c_str())) {
            openDocument(kvitto);
        }
        ImGui::SameLine();
        if (ImGui::Button("X")) {
            to_erase = i;
        }
        i++;
    }
    if (to_erase >= 0) {
        m_attached_kvitton.erase(m_attached_kvitton.begin() + to_erase);    
    }

    ImGui::BeginDisabled(!m_can_attach_kvitto);
    if (ImGui::Button("Attach kvitto")) {
        std::string filename;
        if (fileOpenDialog("pdf;png", filename) == FDR_OKAY) {
            m_attached_kvitton.push_back(std::filesystem::u8path(filename));
        }
    }
    ImGui::EndDisabled();
    ImGui::Separator();
    bool disable_button = false;
    std::string disable_tooltip;

    if (!m_date_ok) {
        disable_button = true;
        disable_tooltip += "Felaktigt datum\n";
    }
    if (m_verifikat->getText().empty()) {
        disable_button = true;
        disable_tooltip += "Text �r tom\n";
    }
    if (balans != 0) {
        disable_button = true;
        disable_tooltip += "Verifikatet balanserar inte\n";
    }
    if (!rader_ok) {
        disable_button = true;
        disable_tooltip += "Fel i en eller flera rader\n";
    }
    if (!locked_ok) {
        disable_button = true;
        disable_tooltip += "Låst konto från Saldo saknas\n";
    }

    ImGui::BeginDisabled(disable_button);
    if (m_dialog_mode == NEW) {
        if (ImGui::Button("Add")) {
            for (size_t i = 0; i < m_konto_rad_data.size(); ++i) {
                // Skip struck rows
                if (m_struken_rad[i].has_value()) {
                    continue;
                }
                int konto_idx = m_konto_rad_data[i].index;
                if (konto_idx >= 0) {
                    int konto = m_konton_id[konto_idx];
                    m_verifikat->addRad(BollDoc::Rad(now(), konto, m_pengar_rad[i]));
                }
            }
            for (const auto& kvitto : m_attached_kvitton) {
                m_file_handler.attachKvitto(m_verifikat->getUnid(), kvitto);
            }
            m_file_handler.getDoc().addVerifikat(std::move(*m_verifikat));
            m_verifikat.release();
            ImGui::CloseCurrentPopup();
        }
    } else if (m_dialog_mode == EDIT) {
        if (ImGui::Button("Update")) {
            std::vector<BollDoc::Rad> rader;
            for (size_t i = 0; i < m_konto_rad_data.size(); ++i) {
                int konto_idx = m_konto_rad_data[i].index;
                if (konto_idx >= 0 && m_pengar_rad[i] != 0) {
                    int konto = m_konton_id[konto_idx];
                    rader.emplace_back(m_bokdatum_rad[i], konto,
                                       m_pengar_rad[i], m_struken_rad[i]);
                }
            }
            int unid = m_verifikat->getUnid();
            for (const auto& kvitto : m_attached_kvitton) {
                m_file_handler.attachKvitto(m_verifikat->getUnid(), kvitto);
            }
            m_file_handler.getDoc().updateVerifikat(std::move(*m_verifikat));
            m_file_handler.getDoc().updateVerifikat(unid, rader);
            m_verifikat.release();
            ImGui::CloseCurrentPopup();
        }
    }
    ImGui::EndDisabled();
    if (disable_button && ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
        ImGui::SetTooltip(disable_tooltip.c_str());
    }
    ImGui::SameLine();
    // Make it impossible to cancel by mistake with the keyboard: the button
    // cannot be reached by tabbing or directional navigation, so it can only
    // be pressed with the mouse.
    ImGui::PushItemFlag(ImGuiItemFlags_NoNav, true);
    if (ImGui::Button("Cancel")) {
        m_verifikat.release();
        ImGui::CloseCurrentPopup();
    }
    ImGui::PopItemFlag();
}
