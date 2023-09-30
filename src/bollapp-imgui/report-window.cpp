#include "report-window.h"

#include <imgui.h>

#include "bolldoc.h"
#include "date.h"
#include "file-handler.h"
#include "report.h"

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

class ResultatReportCache {
   public:
    void update(const BollDoc& doc, const DateRange& range) {
        ResultatRapport report;
        createResultatReport(doc, range, report);
        m_report.clear();
        m_report.reserve(report.m_resultat.size() + 1);
        for (const auto& row : report.m_resultat) {
            RowStrings rs;
            rs.konto = std::to_string(row.first);
            rs.namn = doc.getKonto(row.first).getText();
            rs.res = to_string(row.second);
            m_report.push_back(std::move(rs));
        }
        RowStrings rs;
        rs.namn = "Resultat";
        rs.res = to_string(report.m_sum);
        m_report.push_back(std::move(rs));
    }
    void render() {
        if (ImGui::BeginTable("resultatrapport", 3,
                              ImGuiTableFlags_Resizable | ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY |
                                  ImGuiTableFlags_NoBordersInBody)) {
            ImGui::TableSetupColumn("##kontonr");
            ImGui::TableSetupColumn("Konto");
            ImGui::TableSetupColumn("Resultat");
            ImGui::TableSetupScrollFreeze(0, 1);
            ImGui::TableHeadersRow();

            for (auto& row : m_report) {
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::TextUnformatted(row.konto.c_str());
                ImGui::TableNextColumn();
                ImGui::TextUnformatted(row.namn.c_str());
                ImGui::TableNextColumn();
                imguiTextRightAlign(row.res);
            }
            ImGui::EndTable();
        }
    }

   private:
    struct RowStrings {
        std::string konto;
        std::string namn;
        std::string res;
    };
    std::vector<RowStrings> m_report;
    std::string m_resultat;
};

class BalansReportCache {
   public:
    void update(const BollDoc& doc, const DateRange& range) {
        BalansRapport report;
        createBalansReport(doc, range, report);
        m_report.clear();
        m_report.reserve(report.m_balans.size());
        for (const auto& row : report.m_balans) {
            RowStrings rs;
            rs.konto = std::to_string(row.m_konto);
            rs.namn = doc.getKonto(row.m_konto).getText();
            rs.ib = to_string(row.m_start);
            rs.res = to_string(row.m_end - row.m_start);
            rs.ub = to_string(row.m_end);
            m_report.push_back(std::move(rs));
        }
    }
    void render() {
        if (ImGui::BeginTable("balansrapport", 5,
                              ImGuiTableFlags_Resizable | ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY |
                                  ImGuiTableFlags_NoBordersInBody)) {
            ImGui::TableSetupColumn("##kontonr");
            ImGui::TableSetupColumn("Konto");
            ImGui::TableSetupColumn("IB");
            ImGui::TableSetupColumn("Resultat");
            ImGui::TableSetupColumn("UB");
            ImGui::TableSetupScrollFreeze(0, 1);
            ImGui::TableHeadersRow();

            for (auto& row : m_report) {
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::TextUnformatted(row.konto.c_str());
                ImGui::TableNextColumn();
                ImGui::TextUnformatted(row.namn.c_str());
                ImGui::TableNextColumn();
                imguiTextRightAlign(row.ib);
                ImGui::TableNextColumn();
                imguiTextRightAlign(row.res);
                ImGui::TableNextColumn();
                imguiTextRightAlign(row.ub);
            }
            ImGui::EndTable();
        }
    }

   private:
    struct RowStrings {
        std::string konto;
        std::string namn;
        std::string ib;
        std::string res;
        std::string ub;
    };
    std::vector<RowStrings> m_report;
};

class TagReportCache {
   public:
    void update(const BollDoc& doc, const DateRange& range) {
        std::vector<TaggRow> tagreport;
        createTaggReport(doc, range, tagreport);
        m_tagreport.clear();
        m_tagreport.reserve(tagreport.size());

        for (const auto& row : tagreport) {
            RowStrings rs;

            rs.name = row.m_tagg;

            bool first = true;
            for (int konto : row.m_konton) {
                if (first) {
                    first = false;
                } else {
                    rs.konton.append(", ");
                }
                rs.konton.append(std::to_string(konto));
            }

            rs.ib = to_string(row.m_ib);
            rs.res = to_string(row.m_ub - row.m_ib);
            rs.ub = to_string(row.m_ub);

            m_tagreport.push_back(std::move(rs));
        }
    }

    void render() {
        if (ImGui::BeginTable("taggrapport", 5,
                              ImGuiTableFlags_Resizable | ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY |
                                  ImGuiTableFlags_NoBordersInBody)) {
            ImGui::TableSetupColumn("Tagg");
            ImGui::TableSetupColumn("Konton");
            ImGui::TableSetupColumn("IB");
            ImGui::TableSetupColumn("Resultat");
            ImGui::TableSetupColumn("UB");
            ImGui::TableSetupScrollFreeze(0, 1);
            ImGui::TableHeadersRow();

            for (auto& row : m_tagreport) {
                ImGui::TableNextRow();

                // Tagg
                ImGui::TableSetColumnIndex(0);
                ImGui::TextUnformatted(row.name.c_str());

                // Konton
                ImGui::TableSetColumnIndex(1);
                ImGui::TextUnformatted(row.konton.c_str());

                // IB
                ImGui::TableSetColumnIndex(2);
                imguiTextRightAlign(row.ib);

                // Resultat
                ImGui::TableSetColumnIndex(3);
                imguiTextRightAlign(row.res);

                // UB
                ImGui::TableSetColumnIndex(4);
                imguiTextRightAlign(row.ub);
            }
            ImGui::EndTable();
        }
    }

   private:
    struct RowStrings {
        std::string name;
        std::string konton;
        std::string ib;
        std::string res;
        std::string ub;
    };
    std::vector<RowStrings> m_tagreport;
};

class SaldoReportCache {
   public:
    void update(const BollDoc& doc, const DateRange& range) {
        std::vector<SaldoRow> report;
        createSaldoReport(doc, range, report);
        m_report.clear();
        m_report.reserve(report.size());
        for (const auto& row : report) {
            RowStrings rs;
            rs.konto = std::to_string(row.first);
            rs.namn = doc.getKonto(row.first).getText();
            rs.saldo = to_string(row.second);
            m_report.push_back(std::move(rs));
        }
    }
    void render() {
        if (ImGui::BeginTable("saldorapport", 3,
                              ImGuiTableFlags_Resizable | ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY |
                                  ImGuiTableFlags_NoBordersInBody)) {
            ImGui::TableSetupColumn("##kontonr");
            ImGui::TableSetupColumn("Konto");
            ImGui::TableSetupColumn("Saldo");
            ImGui::TableSetupScrollFreeze(0, 1);
            ImGui::TableHeadersRow();

            for (auto& row : m_report) {
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::TextUnformatted(row.konto.c_str());
                ImGui::TableNextColumn();
                ImGui::TextUnformatted(row.namn.c_str());
                ImGui::TableNextColumn();
                imguiTextRightAlign(row.saldo);
            }
            ImGui::EndTable();
        }
    }

   private:
    struct RowStrings {
        std::string konto;
        std::string namn;
        std::string saldo;
    };
    std::vector<RowStrings> m_report;
};

}  // namespace

class ReportCache {
   public:
    ReportCache() : m_revision(-1), m_range(Date(), Date()), m_report_type(REPORT_RESULTAT) {}
    void update(const BollDoc& doc, const DateRange& range, ReportType report) {
        bool need_update = doc.getRevision() != m_revision;
        need_update = need_update || (range != m_range);
        need_update = need_update || (m_report_type != report);
        if (need_update) {
            m_report_type = report;
            m_revision = doc.getRevision();
            m_range = range;
            switch (m_report_type) {
                case REPORT_BALANS:
                    m_balansreport.update(doc, range);
                    break;
                case REPORT_RESULTAT:
                    m_resultatreport.update(doc, range);
                    break;
                case REPORT_SALDON:
                    m_saldoreport.update(doc, range);
                    break;
                case REPORT_TAGG:
                    m_tagreport.update(doc, range);
                    break;
            }
        }
    }

    void render() {
        switch (m_report_type) {
            case REPORT_BALANS:
                m_balansreport.render();
                break;
            case REPORT_RESULTAT:
                m_resultatreport.render();
                break;
            case REPORT_SALDON:
                m_saldoreport.render();
                break;
            case REPORT_TAGG:
                m_tagreport.render();
                break;
        }
    }

   private:
    int64_t m_revision;
    DateRange m_range;
    ResultatReportCache m_resultatreport;
    BalansReportCache m_balansreport;
    TagReportCache m_tagreport;
    SaldoReportCache m_saldoreport;
    ReportType m_report_type;
};

ReportWindow::ReportWindow(FileHandler& file_handler)
    : ImGuiWindowBase("Rapporter"), m_file_handler(file_handler), m_cache(std::make_unique<ReportCache>()), m_datum(0), m_report_type(0) {}

ReportWindow::~ReportWindow() = default;

void ReportWindow::doit() {
    ImGui::SetNextItemWidth(100);
    if (ImGui::BeginCombo("Datum", dateTypeToString((DateType)m_datum))) {
        for (int n = 0; n < DATETYPE_COUNT; ++n) {
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
    ImGui::SameLine();
    ImGui::SetNextItemWidth(150);
    if (ImGui::BeginCombo("Rapport", reportTypeString((ReportType)m_report_type))) {
        for (int n = 0; n < REPORT_TYPE_COUNT; ++n) {
            bool is_selected = (n == m_report_type);
            if (ImGui::Selectable(reportTypeString((ReportType)n), is_selected)) {
                m_report_type = n;
            }
            if (is_selected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }
    DateRange range = dateTypeToRange((DateType)m_datum, m_file_handler.getDoc().getBokforingsar());
    m_cache->update(m_file_handler.getDoc(), range, (ReportType)m_report_type);
    m_cache->render();
}
