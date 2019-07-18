#include <doctest.h>

#include "serialize.h"

#include <fstream>

TEST_CASE("Serialize load") {
    std::ifstream input("../../../docs/bok1.bollbok");
    REQUIRE(input.good());
    BollDoc doc = Serialize::loadDocument(input);
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
    CHECK(doc.getVerifikat(2).getTransdatum() == Date(2018, 2, 1));
    CHECK_THROWS_WITH(doc.getVerifikat(3), "Verifikat 3 requested, document only has 0-2");

    auto& v = doc.getVerifikat(1);
    CHECK(v.getRad(1).getBokdatum() == Date(2018, 12, 25));
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
