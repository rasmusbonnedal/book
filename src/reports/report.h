#pragma once

#include "pengar.h"

#include <string>
#include <vector>

class BollDoc;
class DateRange;

enum ReportType {
    REPORT_RESULTAT = 0,
    REPORT_BALANS,
    REPORT_TAGG,
    REPORT_SALDON,
    REPORT_TYPE_COUNT
};

const char* reportTypeString(ReportType t);

// Saldo report
using SaldoRow = std::pair<int, Pengar>;

void createSaldoReport(const BollDoc& doc, const DateRange& daterange,
                       std::vector<SaldoRow>& report);

void renderHtmlSaldoReport(const BollDoc& doc,
                           const std::vector<SaldoRow>& report,
                           const DateRange& range, std::ostream& os);

std::string createSaldoReportHtmlFile(const BollDoc& doc,
                                      const DateRange& daterange);

// Resultaträkning report

using ResultatRow = std::pair<int, Pengar>;

struct ResultatRapport {
    std::vector<ResultatRow> m_resultat;
    Pengar m_sum;
};

void createResultatReport(const BollDoc& doc, const DateRange& daterange,
                          ResultatRapport& report);

void renderHtmlResultatReport(const BollDoc& doc,
                              const ResultatRapport& report,
                              const DateRange& range, std::ostream& os);

std::string createResultatReportHtmlFile(const BollDoc& doc,
                                         const DateRange& daterange);

// Balansräkning

struct BalansRow {
    int m_konto;
    Pengar m_start, m_end;
};

struct BalansRapport {
    std::vector<BalansRow> m_balans;
};

void createBalansReport(const BollDoc& doc, const DateRange& daterange,
                        BalansRapport& report);

void renderHtmlBalansReport(const BollDoc& doc, const BalansRapport& report, const DateRange& range, std::ostream& os);

std::string createBalansReportHtmlFile(const BollDoc& doc, const DateRange& daterange);

// Taggrapport

struct TaggRow {
    std::string m_tagg;
    std::vector<int> m_konton;
    Pengar m_ib;
    Pengar m_ub;
};

void createTaggReport(const BollDoc& doc, const DateRange& daterange,
                      std::vector<TaggRow>& report);

void renderHtmlTaggReport(const BollDoc& doc,
                          const std::vector<TaggRow>& report,
                          const DateRange& range, std::ostream& os);

std::string createTaggReportHtmlFile(const BollDoc& doc,
                                     const DateRange& daterange);
