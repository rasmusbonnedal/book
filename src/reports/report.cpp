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
       << "body {" << std::endl
       << "margin: 20px;"
       << "}" << std::endl
       << "table {" << std::endl
       << "width:100%;"
       << "border:1px solid #aaa;"
       << "border-collapse:collapse;"
       << "}" << std::endl
       << "h2 {" << std::endl
       << "	margin:0;"
       << "	font-size:16px;"
       << "	color:#009;"
       << "}" << std::endl
       << "h3 {" << std::endl
       << "	margin:8px 0 1px 0;"
       << "	font-size:13px;"
       << "	color:#009;"
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

// Filter the verifikationer in doc by VerifikatPred, and if it matches
// filter the rows in the verifikat by RowPred.
// Apply Op on all rows that matches both RowPred and VerifikatPred
template <typename VerifikatPred, typename RowPred, typename Op>
void filterVerifikat(const BollDoc& doc, VerifikatPred vpred, RowPred rpred,
                     Op op) {
    for (auto& v : doc.getVerifikationer()) {
        if (vpred(v)) {
            for (auto& r : v.getRader()) {
                if (rpred(r)) {
                    op(r.getKonto(), r.getPengar());
                }
            }
        }
    }
}

} // namespace

void createSaldoReport(const BollDoc& doc, const DateRange& daterange,
                       std::vector<SaldoRow>& report) {
    std::map<int, Pengar> saldoMap;

    auto vpred = [&daterange](auto& v) {
        return (daterange.getEnd() >= v.getTransdatum());
    };
    auto rpred = [](auto& r) { return !r.getStruken(); };
    auto insertOp = [&saldoMap](int konto, Pengar pengar) {
        insertOrAdd(saldoMap, konto, pengar);
    };

    filterVerifikat(doc, vpred, rpred, insertOp);
    report = std::vector<SaldoRow>(saldoMap.begin(), saldoMap.end());
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

    os << "<div>" << std::endl
       << "<h2>Saldon rubrikvis</h2>" << std::endl
       << "</div>" << std::endl
       << "<div>" << std::endl
       << "<h3>" << doc.getFirma() << "</h3>" << std::endl
       << "<p>Räkenskapsår: " << doc.getBokforingsar() << "</p>" << std::endl
       << "<p>Period: " << to_string(range) << "</p>" << std::endl;
    os << "<table>" << std::endl;
    os << "<tr>" << std::endl;
    os << "<th></th>" << std::endl;
    os << "<th>Konto</th>" << std::endl;
    os << "<th style=\"text-align:right\">Saldo</th>" << std::endl;
    os << "</tr>" << std::endl;
    for (auto& row : report) {
        os << "<tr>" << std::endl;
        os << "<td>" << row.first << "</td>" << std::endl;
        os << "<td>" << doc.getKonto(row.first).getText() << "</td>"
           << std::endl;
        os << "<td style=\"text-align:right\">" << toHtmlString(row.second)
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

void createResultatReport(const BollDoc& doc, const DateRange& daterange,
                          std::vector<ResultatRow>& report) {
    std::map<int, Pengar> resultatMap;

    auto vpred = [&daterange](auto& v) {
        return daterange.getEnd() >= v.getTransdatum() &&
               v.getTransdatum() >= daterange.getStart();
    };
    auto rpred = [&doc](auto& r) {
        return !r.getStruken() && doc.getKonto(r.getKonto()).getTyp() == 3;
    };
    auto insertOp = [&resultatMap](int konto, Pengar pengar) {
        insertOrAdd(resultatMap, konto, pengar);
    };

    filterVerifikat(doc, vpred, rpred, insertOp);
    report = std::vector<ResultatRow>(resultatMap.begin(), resultatMap.end());
}

void renderHtmlResultatReport(const BollDoc& doc,
                              const std::vector<SaldoRow>& report,
                              const DateRange& range, std::ostream& os) {
    std::string header = "Resultatrapport " + doc.getFirma() + " (" +
                         doc.getOrgnummer() + ") " + to_string(range);

    htmlDoctype(os);
    os << "<html>" << std::endl;
    htmlHeader(header, os);
    os << "<body>" << std::endl;
    os << "<div>" << std::endl
       << "<h2>Saldon rubrikvis</h2>" << std::endl
       << "</div>" << std::endl
       << "<div>" << std::endl
       << "<h3>" << doc.getFirma() << "</h3>" << std::endl
       << "<p>Räkenskapsår: " << doc.getBokforingsar() << "</p>" << std::endl
       << "<p>Period: " << to_string(range) << "</p>" << std::endl;
    os << "<table>" << std::endl;
    os << "<tr>" << std::endl;
    os << "<th>Konto</th>" << std::endl;
    os << "<th style=\"text-align:right\">Resultat</th>" << std::endl;
    os << "</tr>" << std::endl;
    for (auto& row : report) {
        os << "<tr>" << std::endl;
        os << "<td>" << row.first << "</td>" << std::endl;
        os << "<td>" << doc.getKonto(row.first).getText() << "</td>"
           << std::endl;
        os << "<td style=\"text-align:right\">" << toHtmlString(row.second)
           << ""
           << "</td>" << std::endl;
        os << "</tr>" << std::endl;
    }
    os << "</table>" << std::endl;
    os << "</body>" << std::endl;
    os << "</html>" << std::endl;
}

std::string createResultatReportHtmlFile(const BollDoc& doc,
                                         const DateRange& daterange) {
    srand(time(0));
    std::string filename =
        "/tmp/saldoreport." + std::to_string(rand()) + ".html";
    std::ofstream ofs(filename);
    std::vector<ResultatRow> report;
    createResultatReport(doc, daterange, report);
    renderHtmlResultatReport(doc, report, daterange, ofs);
    return filename;
}
