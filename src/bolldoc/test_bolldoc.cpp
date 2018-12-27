#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <fstream>

#include <doctest.h>

#include "bolldoc.h"

TEST_CASE("Setup") {
    BollDoc doc(2074, "Ruffel & Båg", "551122-1234", 2018, "SEK");
    CHECK(doc.getVersion() == 2074);
    CHECK(doc.getFirma() == "Ruffel & Båg");
    CHECK(doc.getOrgnummer() == "551122-1234");
    CHECK(doc.getBokforingsar() == 2018);
    CHECK(doc.getValuta() == "SEK");
}

TEST_CASE("Parse") {
    std::ifstream input("../../../docs/bok1.bollbok");
    REQUIRE(input.good());
    BollDoc doc = loadDocument(input);
    CHECK(doc.getVersion() == 2074);
    CHECK(doc.getFirma() == "");
    CHECK(doc.getOrgnummer() == "");
    CHECK(doc.getBokforingsar() == 2018);
    CHECK(doc.getValuta() == "SEK");

    CHECK(doc.getKonto(1910).getText() == "Kassa");
    CHECK(doc.getKonto(2010).getTyp() == 1);
    CHECK(!doc.getKonto(2350).getNormalt());
    CHECK(*doc.getKonto(5010).getNormalt() == "+");
    CHECK_THROWS_WITH(doc.getKonto(9999), "Could not find konto 9999");

    CHECK(doc.getVerifikat(1).getText() == "Hyra");
    CHECK(doc.getVerifikat(2).getTransdatum() == "2018-02-01");
    CHECK_THROWS_WITH(doc.getVerifikat(3), "Verifikat 3 requested, document only has 0-2");

    auto& v = doc.getVerifikat(1);
    CHECK(v.getRad(1).getBokdatum() == "2018-12-25");
    CHECK(v.getRad(0).getKonto() == 1910);
    CHECK(v.getRad(0).getPengar() == -800000);
    CHECK_THROWS_WITH(doc.getVerifikat(1).getRad(2), "Rad 2 requested, verifikat only has 0-1");

    for (auto&& v : doc.getVerifikat()) {
        int64_t sum = 0;
        for (auto&& r : v.getRader()) {
            sum += r.getPengar();
        }
        CHECK(sum == 0);
    }
}
