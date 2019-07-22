#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>

#include "bolldoc.h"

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
        return doc;
    }
}

TEST_CASE("Setup") {
    BollDoc doc = createDoc();
    CHECK(doc.getVersion() == 2074);
    CHECK(doc.getFirma() == "Ruffel & Båg");
    CHECK(doc.getOrgnummer() == "551122-1234");
    CHECK(doc.getBokforingsar() == 2018);
    CHECK(doc.getValuta() == "SEK");
    CHECK(doc.getAvslutat() == false);
}

TEST_CASE("Ranges") {
    BollDoc doc = createDoc();
    for (int i = 1; i <= 12; ++i) {
        Date endDate = i < 12 ? Date(2018, i + 1, 1) : Date(2019, 1, 1);
        auto verifikat = doc.getVerifikatRange(Date(2018, i, 1), endDate);
        REQUIRE(verifikat.size() == 1);
        CHECK(verifikat[0]->getTransdatum() == Date(2018, i, 1));
    }
    for (int i = 1; i <= 12; i += 3) {
        Date startDate = Date(2018, i, 1);
        Date endDate = i < 10 ? Date(2018, i + 3, 1) : Date(2019, 1, 1);
        auto verifikat = doc.getVerifikatRange(startDate, endDate);
        REQUIRE(verifikat.size() == 3);
        CHECK(verifikat[0]->getTransdatum() == Date(2018, i, 1));
        CHECK(verifikat[1]->getTransdatum() == Date(2018, i + 1, 1));
        CHECK(verifikat[2]->getTransdatum() == Date(2018, i + 2, 1));
    }
    CHECK(doc.getVerifikatRange(0, 4).size() == 4);
    CHECK(doc.getVerifikatRange(4, 7).size() == 3);
    CHECK(doc.getVerifikatRange(7, 11).size() == 4);
    CHECK(doc.getVerifikatRange(11, 13).size() == 2);
}
