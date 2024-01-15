#include "one-verifikat-window.h"

#include <imgui.h>

#include "bolldoc.h"
#include "book-app.h"
#include "file-handler.h"
#include "saldo-window.h"

namespace {
void imguiRightAlign(const char* text) {
    auto posX = (ImGui::GetCursorPosX() + ImGui::GetColumnWidth() - ImGui::CalcTextSize(text).x - ImGui::GetScrollX() -
                 0 * ImGui::GetStyle().ItemSpacing.x);
    if (posX > ImGui::GetCursorPosX()) {
        ImGui::SetCursorPosX(posX);
    }
}

void imguiTextRightAlign(const std::string& s) {
    imguiRightAlign(s.c_str());
    ImGui::TextUnformatted(s.c_str());
}

const int DEFAULT_SALDO_WIDTH = 90;
}  // namespace

OneVerifikatWindow::OneVerifikatWindow(FileHandler& file_handler, BookApp& app)
    : ImGuiWindowBase("Ett verifikat", ImVec2(60, 10), ImVec2(80, 15)),
      _file_handler(file_handler),
      _app(app),
      _revision(-1),
      _verifikat(-1) {}

void OneVerifikatWindow::setVerifikat(int v) {
    _verifikat = v;
    update(true);
}

void OneVerifikatWindow::doit() {
    bool need_update = false;
    if (ImGui::InputInt("Verifikat", &_verifikat)) {
        need_update = true;
    }
    update(need_update);

    if (_verifikat < 0) {
        return;
    }
    ImGui::TextUnformatted("Text: ");
    ImGui::SameLine();
    ImGui::TextUnformatted(_text.c_str());
    ImGui::TextUnformatted("Datum: ");
    ImGui::SameLine();
    ImGui::TextUnformatted(_datum.c_str());
    if (ImGui::BeginTable("ettverifikat", 3,
                          ImGuiTableFlags_Resizable | ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY |
                              ImGuiTableFlags_NoBordersInBody)) {
        ImGui::TableSetupColumn("##kontonr", ImGuiTableColumnFlags_WidthFixed, 30);
        ImGui::TableSetupColumn("Konto");
        ImGui::TableSetupColumn("Saldo", ImGuiTableColumnFlags_WidthFixed, DEFAULT_SALDO_WIDTH);
        ImGui::TableSetupScrollFreeze(0, 1);
        ImGui::TableHeadersRow();
        for (auto& row : _rows) {
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            if (ImGui::Button(row.konto.c_str())) {
                _app.saldoWindow().setKonto(row.konto_id);
            }
            ImGui::TableNextColumn();
            ImGui::TextUnformatted(row.namn.c_str());
            ImGui::TableNextColumn();
            imguiTextRightAlign(row.saldo.c_str());
        }

        ImGui::EndTable();
    }
}

void OneVerifikatWindow::update(bool need_update) {
    if (_verifikat < 0) {
        return;
    }
    need_update = need_update || (_revision != _file_handler.getDoc().getRevision());
    if (!need_update) {
        return;
    }

    _revision = _file_handler.getDoc().getRevision();
    _rows.clear();
    const auto& vers = _file_handler.getDoc().getVerifikationer();
    if (_verifikat < vers.size()) {
        const auto& ver = vers[_verifikat];
        _text = ver.getText();
        _datum = to_string(ver.getTransdatum());
        for (const auto& row : ver.getRader()) {
            Row srow;
            srow.konto_id = row.getKonto();
            srow.konto = std::to_string(row.getKonto());
            srow.namn = _file_handler.getDoc().getKonto(row.getKonto()).getText();
            srow.saldo = to_string(row.getPengar());
            _rows.push_back(std::move(srow));
        }
    }
}
