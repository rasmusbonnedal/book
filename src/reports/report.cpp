#include "report.h"

#include "bolldoc.h"
#include "date.h"

#include <cassert>
#include <ctime>
#include <fstream>
#include <map>

namespace {
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
       << "p {" << std::endl
       << "padding:1px 2px;"
       << "margin:0;"
       << "}" << std::endl
       << "tr:nth-child(even) {background: #EEE}" << std::endl
       << "tr:nth-child(odd) {background: #FFF}" << std::endl
       << "td:first-child { width:30px; }" << std::endl
       << ".pengar {" << std::endl
       << " white-space: nowrap;"
       << " text-align:right;"
       << "}" << std::endl
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
                    op(r.getKonto(), r.getPengar(), v.getTransdatum());
                }
            }
        }
    }
}

std::string getTempFile(const std::string& name) {
    char* tmp;
    std::string tempdir;
    if ((tmp = getenv("TMPDIR")) != 0) {
        tempdir = tmp;
    } else if ((tmp = getenv("TMP")) != 0) {
        tempdir = tmp;
    } else if ((tmp = getenv("TEMP")) != 0) {
        tempdir = tmp;
    } else {
#if defined(_WIN32)
        tempdir = "C:\\";
#else
        tempdir = "/tmp";
#endif
    }
    return tempdir + "/" + name;
}

} // namespace

std::string reportTypeString(ReportType t) {
    switch (t) {
    case REPORT_RESULTAT:
        return "Resultaträkning";
    case REPORT_BALANS:
        return "Balansräkning";
    case REPORT_TAGG:
        return "Taggrapport";
    case REPORT_SALDON:
        return "Saldon";
    case REPORT_TYPE_COUNT:;
    }
    throw std::runtime_error("Invalid ReportType " + std::to_string(t));
}

void createSaldoReport(const BollDoc& doc, const DateRange& daterange,
                       std::vector<SaldoRow>& report) {
    std::map<int, Pengar> saldoMap;

    auto vpred = [&daterange](auto& v) {
        return (daterange.getEnd() >= v.getTransdatum());
    };
    auto rpred = [](auto& r) { return !r.getStruken(); };
    auto insertOp = [&saldoMap](int konto, Pengar pengar, Date) {
        saldoMap[konto] += pengar;
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
    srand((unsigned int)time(0));
    std::string filename =
        getTempFile("saldoreport." + std::to_string(rand()) + ".html");
    std::ofstream ofs(filename);
    std::vector<SaldoRow> report;
    createSaldoReport(doc, daterange, report);
    renderHtmlSaldoReport(doc, report, daterange, ofs);
    return filename;
}

void createResultatReport(const BollDoc& doc, const DateRange& daterange,
                          ResultatRapport& report) {
    std::map<int, Pengar> resultatMap;

    auto vpred = [&daterange](auto& v) {
        return daterange.getEnd() >= v.getTransdatum() &&
               v.getTransdatum() >= daterange.getStart();
    };
    auto rpred = [&doc](auto& r) {
        return !r.getStruken() && doc.getKonto(r.getKonto()).getTyp() == 3;
    };
    auto insertOp = [&resultatMap](int konto, Pengar pengar, Date) {
        resultatMap[konto] += pengar;
    };

    filterVerifikat(doc, vpred, rpred, insertOp);
    report.m_resultat = std::vector<ResultatRow>(resultatMap.begin(), resultatMap.end());
    for (auto& x : report.m_resultat) {
        report.m_sum += x.second;
    }
}

void renderHtmlResultatReport(const BollDoc& doc,
                              const ResultatRapport& report,
                              const DateRange& range, std::ostream& os) {
    std::string header = "Resultaträkning " + doc.getFirma() + " (" +
                         doc.getOrgnummer() + ") " + to_string(range);

    htmlDoctype(os);
    os << "<html>" << std::endl;
    htmlHeader(header, os);
    os << "<body>" << std::endl;
    os << "<div>" << std::endl
       << "<h2>Resultaträkning</h2>" << std::endl
       << "</div>" << std::endl
       << "<div>" << std::endl
       << "<h3>" << doc.getFirma() << "</h3>" << std::endl
       << "<p>Räkenskapsår: " << doc.getBokforingsar() << "</p>" << std::endl
       << "<p>Period: " << to_string(range) << "</p>" << std::endl;
    os << "<table>" << std::endl;
    os << "<tr>" << std::endl;
    os << "<th></th>" << std::endl;
    os << "<th>Konto</th>" << std::endl;
    os << "<th style=\"text-align:right\">Förändring</th>" << std::endl;
    os << "</tr>" << std::endl;
    for (auto& row : report.m_resultat) {
        os << "<tr>" << std::endl;
        os << "<td>" << row.first << "</td>" << std::endl;
        os << "<td>" << doc.getKonto(row.first).getText() << "</td>"
           << std::endl;
        os << "<td style=\"text-align:right\">" << toHtmlString(row.second)
           << "</td>" << std::endl;
        os << "</tr>" << std::endl;
    }
    os << "<tr>" << std::endl;
    os << "<td></td>" << std::endl;
    os << "<td>Summa</td>" << std::endl;
    os << "<td style=\"text-align:right\">" << toHtmlString(report.m_sum)
       << "</td>" << std::endl;
    os << "</table>" << std::endl;
    os << "<h3>Resultat</h3>"
       << "<table>" << std::endl
       << "<tr><td></td><td>Resultat</td><td style=\"text-align:right\">"
       << toHtmlString(-report.m_sum) << "</td></tr>" << std::endl
       << "</table>" << std::endl;
    os << "</body>" << std::endl;
    os << "</html>" << std::endl;
}

std::string createResultatReportHtmlFile(const BollDoc& doc,
                                         const DateRange& daterange) {
    srand((unsigned int)time(0));
    std::string filename =
        getTempFile("resultreport." + std::to_string(rand()) + ".html");
    std::ofstream ofs(filename);
    ResultatRapport report;
    createResultatReport(doc, daterange, report);
    renderHtmlResultatReport(doc, report, daterange, ofs);
    return filename;
}

void createBalansReport(const BollDoc& doc, const DateRange& daterange,
                        BalansRapport& report) {
    std::map<int, std::pair<Pengar, Pengar>> balansMap;

    auto vpred = [&daterange](auto& v) {
        return daterange.getEnd() >= v.getTransdatum();
    };
    auto rpred = [&doc](auto& r) {
        return !r.getStruken() && doc.getKonto(r.getKonto()).getTyp() == 1;
    };
    auto insertOp = [&balansMap, &daterange](int konto, Pengar pengar, Date date) {
        if (date < daterange.getStart()) {
            balansMap[konto].first += pengar;
        }
        balansMap[konto].second += pengar;
    };

    filterVerifikat(doc, vpred, rpred, insertOp);
    for (auto& it: balansMap) {
        report.m_balans.push_back({ it.first, it.second.first, it.second.second });
    }
}

void renderHtmlBalansReport(const BollDoc& doc, const BalansRapport& report,
                            const DateRange& range, std::ostream& os) {
    std::string header = "Balansräkning " + doc.getFirma() + " (" +
                         doc.getOrgnummer() + ") " + to_string(range);

    htmlDoctype(os);
    os << "<html>" << std::endl;
    htmlHeader(header, os);
    os << "<body>" << std::endl;
    os << "<div>" << std::endl
       << "<h2>Balansräkning</h2>" << std::endl
       << "</div>" << std::endl
       << "<div>" << std::endl
       << "<h3>" << doc.getFirma() << "</h3>" << std::endl
       << "<p>Räkenskapsår: " << doc.getBokforingsar() << "</p>" << std::endl
       << "<p>Period: " << to_string(range) << "</p>" << std::endl;
    os << "<table>" << std::endl;
    os << "<tr>" << std::endl;
    os << "<th></th>" << std::endl;
    os << "<th>Konto</th>" << std::endl;
    os << "<th style=\"text-align:right\">Ingående</th>" << std::endl;
    os << "<th style=\"text-align:right\">Förändring</th>" << std::endl;
    os << "<th style=\"text-align:right\">Utgående</th>" << std::endl;
    os << "</tr>" << std::endl;
    Pengar startSum(0), endSum(0);
    for (auto& row : report.m_balans) {
        os << "<tr>" << std::endl;
        os << "<td>" << row.m_konto << "</td>" << std::endl;
        os << "<td>" << doc.getKonto(row.m_konto).getText() << "</td>"
           << std::endl;
        os << "<td style=\"text-align:right\">" << toHtmlString(row.m_start)
           << "</td>" << std::endl;
        os << "<td style=\"text-align:right\">" << toHtmlString(row.m_end - row.m_start)
           << "</td>" << std::endl;
        os << "<td style=\"text-align:right\">" << toHtmlString(row.m_end)
           << "</td>" << std::endl;
        os << "</tr>" << std::endl;
        startSum += row.m_start;
        endSum += row.m_end;
    }
    os << "<tr>" << std::endl;
    os << "<td></td>" << std::endl;
    os << "<td>Summa</td>" << std::endl;
    os << "<td style=\"text-align:right\">" << toHtmlString(startSum)
       << "</td>" << std::endl;
    os << "<td style=\"text-align:right\">" << toHtmlString(endSum - startSum)
       << "</td>" << std::endl;
    os << "<td style=\"text-align:right\">" << toHtmlString(endSum)
       << "</td>" << std::endl;
    os << "</table>" << std::endl;
    os << "</body>" << std::endl;
    os << "</html>" << std::endl;
}

std::string createBalansReportHtmlFile(const BollDoc& doc,
                                       const DateRange& daterange) {
    srand((unsigned int)time(0));
    std::string filename =
        getTempFile("balansreport." + std::to_string(rand()) + ".html");
    std::ofstream ofs(filename);
    BalansRapport report;
    createBalansReport(doc, daterange, report);
    renderHtmlBalansReport(doc, report, daterange, ofs);
    return filename;
}

void createTaggReport(const BollDoc& doc, const DateRange& daterange,
                      std::vector<TaggRow>& report) {
    std::map<std::string, std::vector<int>> taggMap;
    for (auto& konto : doc.getKontoPlan()) {
        const auto& tagg = konto.second.getTagg();
        if (tagg) {
            taggMap[*tagg].push_back(konto.second.getUnid());
        }
    }

    std::map<std::string, Pengar> taggIb;
    auto vpredIb = [&daterange](auto& v) {
        return v.getTransdatum() < daterange.getStart();
    };
    auto rpred = [&doc](auto& r) {
        return !r.getStruken() && doc.getKonto(r.getKonto()).getTagg();
    };
    auto insertOpIb = [&doc, &taggIb](int konto, Pengar pengar, Date) {
        taggIb[*doc.getKonto(konto).getTagg()] += pengar;
    };
    filterVerifikat(doc, vpredIb, rpred, insertOpIb);

    std::map<std::string, Pengar> taggUb;
    auto vpredUb = [&daterange](auto& v) {
        return daterange.getEnd() >= v.getTransdatum();
    };
    auto insertOpUb = [&doc, &taggUb](int konto, Pengar pengar, Date) {
        taggUb[*doc.getKonto(konto).getTagg()] += pengar;
    };
    filterVerifikat(doc, vpredUb, rpred, insertOpUb);

    std::map<std::string, Pengar> taggResultat;
    auto vpredRes = [&daterange](auto& v) {
        return v.getTransdatum() >= daterange.getStart() &&
        daterange.getEnd() >= v.getTransdatum();
    };
    auto insertOpRes = [&doc, &taggResultat](int konto, Pengar pengar, Date) {
        taggResultat[*doc.getKonto(konto).getTagg()] += pengar;
    };
    filterVerifikat(doc, vpredRes, rpred, insertOpRes);

    report.clear();
    report.reserve(taggMap.size());
    for (auto& tagg : taggMap) {
        TaggRow r = { tagg.first, std::move(tagg.second), taggIb[tagg.first], taggUb[tagg.first]};
        report.push_back(std::move(r));
        assert(taggUb[tagg.first] + (-taggIb[tagg.first]) == taggResultat[tagg.first]);
    }
}

void renderHtmlTaggReport(const BollDoc& doc,
                          const std::vector<TaggRow>& report,
                          const DateRange& range, std::ostream& os) {
    std::string header = "Taggrapport " + doc.getFirma() + " (" +
                         doc.getOrgnummer() + ") " + to_string(range);

    htmlDoctype(os);
    os << "<html>" << std::endl;
    htmlHeader(header, os);
    os << "<body>" << std::endl;
    os << "<div>" << std::endl
       << "<h2>Taggrapport</h2>" << std::endl
       << "</div>" << std::endl
       << "<div>" << std::endl
       << "<h3>" << doc.getFirma() << "</h3>" << std::endl
       << "<p>Räkenskapsår: " << doc.getBokforingsar() << "</p>" << std::endl
       << "<p>Period: " << to_string(range) << "</p>" << std::endl;
    os << "<table>" << std::endl;
    os << "<tr>" << std::endl;
    os << "<th>Kod</th>" << std::endl;
    os << "<th>Konton</th>" << std::endl;
    os << "<th class=\"pengar\">" << to_string(range.getStart()) << "</th>" << std::endl;
    os << "<th class=\"pengar\">Förändring</th>" << std::endl;
    os << "<th class=\"pengar\">" << to_string(range.getEnd()) << "</th>" << std::endl;
    os << "</tr>" << std::endl;
    for (auto& row : report) {
        os << "<tr>" << std::endl;
        os << "<td>" << row.m_tagg << "</td>" << std::endl;
        os << "<td>";
        bool first = true;
        for (int konto : row.m_konton) {
            if (first) {
                first = false;
            } else {
                os << ", ";
            }
            os << konto;
        }
        os << "</td>" << std::endl;
        os << "<td class=\"pengar\">" << toHtmlString(row.m_ib) << "</td>"
           << std::endl;
        os << "<td class=\"pengar\">" << toHtmlString(row.m_ub - row.m_ib)
           << "</td>" << std::endl;
        os << "<td class=\"pengar\">" << toHtmlString(row.m_ub) << "</td>"
           << std::endl;
        os << "</tr>" << std::endl;
    }
    os << "</table>" << std::endl;
    os << "</body>" << std::endl;
    os << "</html>" << std::endl;
}

std::string createTaggReportHtmlFile(const BollDoc& doc,
                                     const DateRange& daterange) {
    srand((unsigned int)time(0));
    std::string filename =
        getTempFile("taggreport." + std::to_string(rand()) + ".html");
    std::ofstream ofs(filename);
    std::vector<TaggRow> report;
    createTaggReport(doc, daterange, report);
    renderHtmlTaggReport(doc, report, daterange, ofs);
    return filename;
}
