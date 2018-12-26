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
    CHECK_THROWS_WITH(doc.getKonto(9999), "Could not find konto 9999");
    CHECK(doc.getKonto(2010).getTyp() == 1);
    CHECK(!doc.getKonto(2350).getNormalt());
    CHECK(*doc.getKonto(5010).getNormalt() == "+");
}
