#include <doctest.h>

#include "serialize.h"
#include "utils.h"

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
    CHECK(!doc.getKonto(2350).getTagg());
    CHECK(*doc.getKonto(3000).getTagg() == "F");
    CHECK_THROWS_WITH(doc.getKonto(9999), "Could not find konto 9999");

    CHECK(doc.getVerifikat(1).getText() == "Hyra");
    CHECK(doc.getVerifikat(2).getTransdatum() == Date(2018, 2, 1));
    CHECK_THROWS_WITH(doc.getVerifikat(4), "Verifikat 4 requested, document only has 0-3");

    auto& v1 = doc.getVerifikat(1);
    CHECK(v1.getRad(1).getBokdatum() == Date(2018, 12, 25));
    CHECK(v1.getRad(0).getKonto() == 1910);
    CHECK(v1.getRad(0).getPengar().get() == -800000);
    CHECK_THROWS_WITH(v1.getRad(2), "Rad 2 requested, verifikat only has 0-1");

    auto& v2 = doc.getVerifikat(2);
    CHECK(v2.getRad(1).getStruken());

    for (auto&& v : doc.getVerifikationer()) {
        int64_t sum = 0;
        for (auto&& r : v.getRader()) {
            if (!r.getStruken()) sum += r.getPengar().get();
        }
        CHECK(sum == 0);
    }
}

TEST_CASE("Serialize save") {
    BollDoc doc(2074, "Ruffel & Båg", "551122-1234", 2018, "SEK", true);
    doc.addKonto(BollDoc::Konto(1110, "Byggnader", 1));
    doc.addKonto(BollDoc::Konto(3000, "Försäljning inom Sverige", 3, "-", "F"));

    BollDoc::Verifikat v1{0, "Ingående saldon", parseDate("0000-01-01")};
    v1.addRad({parseDate("2018-12-25"), 1110, parsePengar("1000000")});
    v1.addRad({parseDate("2018-12-25"), 2350, parsePengar("-800000")});
    v1.addRad({parseDate("2018-12-25"), 2010, parsePengar("-300000")});
    v1.addRad({parseDate("2018-12-25"), 1910, parsePengar("100000")});
    doc.addVerifikat(std::move(v1));

    BollDoc::Verifikat v2{1, "Hyra", parseDate("2018-01-01")};
    v2.addRad({parseDate("2018-12-25"), 1910, parsePengar("-8000")});
    v2.addRad({parseDate("2018-12-25"), 5010, parsePengar("8000")});
    doc.addVerifikat(std::move(v2));

    BollDoc::Verifikat v3{2, "Försäljning", parseDate("2018-02-01")};
    v3.addRad({parseDate("2018-12-25"), 1910, parsePengar("3000")});
    v3.addRad({parseDate("2018-12-25"), 3000, parsePengar("-3000"), parseDate("2019-07-21")});
    v3.addRad({parseDate("2019-07-21"), 3001, parsePengar("-3000")});
    doc.addVerifikat(std::move(v3));

    std::ofstream output("../../../docs/output.bollbok");
    REQUIRE(output.good());
    Serialize::saveDocument(doc, output);
}
