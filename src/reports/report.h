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
