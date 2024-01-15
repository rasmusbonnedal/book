#include "saldo-window.h"

#include <imgui.h>

#include "bolldoc.h"
#include "book-app.h"
#include "one-verifikat-window.h"

namespace {
void imguiRightAlign(const char* text) {
    auto posX = (ImGui::GetCursorPosX() + ImGui::GetColumnWidth() - ImGui::CalcTextSize(text).x - ImGui::GetScrollX() -
                 0 * ImGui::GetStyle().ItemSpacing.x);
    if (posX > ImGui::GetCursorPosX()) {
        ImGui::SetCursorPosX(posX);
    }
}
}  // namespace

SaldoWindow::SaldoWindow(FileHandler& file_handler, BookApp& app)
    : ImGuiWindowBase("Saldo", ImVec2(80, 10), ImVec2(0, 30)), _file_handler(file_handler), _app(app) {
    _cache_rev = 0xffffffff;
    makeSelectData();
}

void SaldoWindow::makeSelectData() {
    _konton.clear();
    _konton_id.clear();
    for (const auto& [k, v] : _file_handler.getDoc().getKontoPlan()) {
        _konton.push_back(std::to_string(k) + " " + v.getText());
        _konton_id.push_back(k);
    }
    _konto_select = std::make_unique<ImGui::ComboAutoSelectData>(_konton, -1);
}

void SaldoWindow::doit() {
    const auto& doc = _file_handler.getDoc();
    if (doc.getKontoPlan().size() != _konton.size()) {
        makeSelectData();
    }
    if (ImGui::ComboAutoSelect("Konto", *_konto_select, _konton, 0)) {
    }
    if (_konto_select->index < 0) {
        return;
    }
    if (ImGui::BeginTable("verifikat", 4,
                          ImGuiTableFlags_Resizable | ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg |
                              ImGuiTableFlags_ScrollY | ImGuiTableFlags_NoBordersInBody)) {
        ImGui::TableSetupColumn("Datum");
        ImGui::TableSetupColumn("Beskrivning");
        ImGui::TableSetupColumn("Belopp");
        ImGui::TableSetupColumn("Saldo");
        ImGui::TableSetupScrollFreeze(0, 1);
        ImGui::TableHeadersRow();
        
        if (doc.getRevision() == _cache_rev && _verifikat_cache.size() != doc.getVerifikationer().size()) {
            std::cout << "Warning: _cache_rev equals but vector size mismatch!" << std::endl;
            _cache_rev = 0xffffffff;
        }
        if (doc.getRevision() != _cache_rev) {
            _cache_rev = doc.getRevision();
            _verifikat_cache = doc.getVerifikationer();
            std::sort(_verifikat_cache.begin(), _verifikat_cache.end(), [](auto& lhs, auto& rhs) -> bool {
                if (lhs.getTransdatum() == rhs.getTransdatum()) {
                    return lhs.getUnid() < rhs.getUnid();
                }
                return lhs.getTransdatum() < rhs.getTransdatum();
            });
        }

        Pengar saldo;
        char display_buf[256];
        for (auto& v : _verifikat_cache) {
            for (auto& rad : v.getRader()) {
                if (!rad.getStruken() && rad.getKonto() == _konton_id[_konto_select->index]) {
                    saldo = saldo + rad.getPengar();
                    ImGui::TableNextRow();
                    // Datum
                    ImGui::TableSetColumnIndex(0);
                    to_string(v.getTransdatum(), display_buf);
                    ImGui::TextUnformatted(display_buf);

                    // Beskrivning
                    ImGui::TableSetColumnIndex(1);
                    if (ImGui::Button(v.getText().c_str())) {
                        _app.oneVerifikatWindow().setVerifikat(v.getUnid());
                    }

                    // Belopp
                    ImGui::TableSetColumnIndex(2);
                    to_string(rad.getPengar(), display_buf);
                    imguiRightAlign(display_buf);
                    ImGui::TextUnformatted(display_buf);

                    // Saldo
                    ImGui::TableSetColumnIndex(3);
                    to_string(saldo, display_buf);
                    imguiRightAlign(display_buf);
                    ImGui::TextUnformatted(display_buf);
                }
            }        
        }
        ImGui::EndTable();
    }
}

void SaldoWindow::setKonto(int konto) {
    for (int i = 0; i < _konton_id.size(); ++i) {
        if (_konton_id[i] == konto) {
            _konto_select->index = i;
            return;
        }
    }
}