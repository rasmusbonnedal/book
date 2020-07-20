#include "report.h"

#include "bolldoc.h"
#include "date.h"

#include <fstream>
#include <map>

namespace {
template <class Key, class Value>
void insertOrAdd(std::map<Key, Value>& m, const Key& k, const Value& v) {
    auto it = m.find(k);
    if (it == m.end()) {
        m[k] = v;
    } else {
        it->second += v;
    }
}

void htmlDoctype(std::ostream& os) {
    os << "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01//EN\" "
          "\"http://www.w3.org/TR/html4/strict.dtd\">"
       << std::endl;
}

void cssFont(std::ostream& os) {
    os << "html, body, table, tr, td, th {" << std::endl
       << "font: 100% arial;" << std::endl
       << "font-size: small;"
       << "text-align: left;"
       << "margin: 8px;"
       << "padding: 2px;"
       << "}" << std::endl
       << "tr:nth-child(even) {background: #EEE}" << std::endl
       << "tr:nth-child(odd) {background: #FFF}" << std::endl
       << "th {font-weight: bold;}";
}

void htmlHeader(const std::string& title, std::ostream& os) {
    os << "<head>" << std::endl
       << "<title>" << title << "</title>" << std::endl
       << "<style>" << std::endl;
    cssFont(os);
    os << "</style>" << std::endl << "</head>" << std::endl;
}

} // namespace

void createSaldoReport(const BollDoc& doc, const DateRange& daterange,
                       std::vector<SaldoRow>& report) {
    std::map<int, Pengar> saldoMap;
    for (auto& v : doc.getVerifikationer()) {
        if (daterange.getEnd() >= v.getTransdatum()) {
            for (auto& r : v.getRader()) {
                if (!r.getStruken()) {
                    insertOrAdd(saldoMap, r.getKonto(), r.getPengar());
                }
            }
        }
    }
    report.clear();
    for (auto& it : saldoMap) {
        report.emplace_back(it.first, it.second);
    }
}

void renderHtmlSaldoReport(const BollDoc& doc,
                           const std::vector<SaldoRow>& report,
                           const DateRange& range, std::ostream& os) {
    std::string header = "Saldon för " + doc.getFirma() + " (" +
                         doc.getOrgnummer() + ") " + to_string(range);

    htmlDoctype(os);
    os << "<html>" << std::endl;
    htmlHeader(header, os);
    os << "<body>" << std::endl;

    os << "<table>" << std::endl;
    os << "<tr>" << std::endl;
    os << "<th>Konto</th>" << std::endl;
    os << "<th>Saldo</th>" << std::endl;
    os << "</tr>" << std::endl;
    for (auto& row : report) {
        os << "<tr>" << std::endl;
        os << "<td>" << row.first << " " << doc.getKonto(row.first).getText()
           << "</td>" << std::endl;
        os << "<td style=\"text-align:right\">" << row.second << " kr"
           << "</td>" << std::endl;
        os << "</tr>" << std::endl;
    }
    os << "</table>" << std::endl;
    os << "</body>" << std::endl;
    os << "</html>" << std::endl;
}

std::string createSaldoReportHtmlFile(const BollDoc& doc,
                                      const DateRange& daterange) {
    srand(time(0));
    std::string filename =
        "/tmp/saldoreport." + std::to_string(rand()) + ".html";
    std::ofstream ofs(filename);
    std::vector<SaldoRow> report;
    createSaldoReport(doc, daterange, report);
    renderHtmlSaldoReport(doc, report, daterange, ofs);
    return filename;
}
