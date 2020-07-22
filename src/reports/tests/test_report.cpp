#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>

#include "bolldoc.h"
#include "report.h"

namespace {
BollDoc createDoc() {
    BollDoc doc(2074, "Ruffel & Båg", "551122-1234", 2018, "SEK", false);

    doc.addVerifikat({0, "Ingående saldon", Date(0, 1, 1)});
    for (int i = 1; i <= 12; ++i) {
        BollDoc::Verifikat v{i, "Hyra", Date(2018, i, 1)};
        v.addRad({Date(2018, 12, 25), 1910, parsePengar("-8000")});
        v.addRad({Date(2018, 12, 25), 5010, parsePengar("8000")});
        doc.addVerifikat(std::move(v));
    }
    doc.addKonto({1910, "Bankkonto", 1});
    doc.addKonto({5010, "Byggkostnader", 3});
    return doc;
}
} // namespace

TEST_CASE("Report Saldo") {
    std::vector<SaldoRow> report;
    BollDoc doc = createDoc();
    DateRange range(Date(2018, 1, 1), Date(2018, 3, 1));
    createSaldoReport(doc, range, report);
    CHECK(report.size() == 2);
    CHECK(report[0].first == 1910);
    CHECK(report[0].second == Pengar(-2400000));
    CHECK(report[1].first == 5010);
    CHECK(report[1].second == Pengar(2400000));
    std::stringstream ss;
    renderHtmlSaldoReport(doc, report, range, ss);
    std::cout << ss.str();
}

TEST_CASE("Report Resultat") {
    std::vector<ResultatRow> report;
    BollDoc doc = createDoc();
    DateRange range(Date(2018, 1, 1), Date(2018, 3, 1));
    createResultatReport(doc, range, report);
    CHECK(report.size() == 1);
    CHECK(report[0].first == 5010);
    CHECK(report[0].second == Pengar(2400000));
    std::stringstream ss;
    renderHtmlResultatReport(doc, report, range, ss);
    std::cout << ss.str();
}

TEST_CASE("Report Saldo html") {
    BollDoc doc = createDoc();
    DateRange range(Date(2018, 1, 1), Date(2018, 6, 1));
    std::string filename = createSaldoReportHtmlFile(doc, range);
    std::cout << filename << std::endl;
    remove(filename.c_str());
}
