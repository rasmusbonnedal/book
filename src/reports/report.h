#pragma once

#include "pengar.h"

#include <string>
#include <vector>

class BollDoc;
class DateRange;

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
