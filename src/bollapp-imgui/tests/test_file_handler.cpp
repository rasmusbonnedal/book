#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>

#include <sstream>

#include "bolldoc.h"
#include "file-handler.h"
#include "sieparse.h"

TEST_CASE("Bokforingsorder is not exported to SIE") {
    BollDoc doc(2074, "Ruffel & Båg", "551122-1234", 2018, "SEK", false);
    doc.addVerifikat({0, "Ingående saldon", Date(0, 1, 1)});

    BollDoc::Verifikat verifikat{1, "Bokförd hyra", Date(2018, 2, 1)};
    verifikat.addRad({Date(2018, 2, 1), 1910, parsePengar("-8000")});
    verifikat.addRad({Date(2018, 2, 1), 5010, parsePengar("8000")});
    doc.addVerifikat(std::move(verifikat));

    BollDoc::Verifikat bokforingsorder{
        2, "Preliminar hyra", Date(2018, 3, 1), true};
    bokforingsorder.addRad(
        {Date(2018, 3, 1), 1910, parsePengar("-9000")});
    bokforingsorder.addRad(
        {Date(2018, 3, 1), 5010, parsePengar("9000")});
    doc.addVerifikat(std::move(bokforingsorder));

    doc.addOrUpdateKonto({1910, "Bankkonto", 1});
    doc.addOrUpdateKonto({5010, "Hyra", 3});

    std::stringstream exported;
    REQUIRE(export_sie(doc, exported));

    SIEData siedata;
    REQUIRE(parse(siedata, exported));
    REQUIRE(siedata.verifikat.size() == 1);
    CHECK(siedata.verifikat[0].id == 1);
    CHECK(siedata.verifikat[0].text == "Bokförd hyra");
}
