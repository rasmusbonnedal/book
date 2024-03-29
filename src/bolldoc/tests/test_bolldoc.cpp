#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>

#include "bolldoc.h"

namespace {
BollDoc createDoc() {
    BollDoc doc(2074, "Ruffel & Båg", "551122-1234", 2018, "SEK", false);

    doc.addOrUpdateKonto(BollDoc::Konto(1910, "Kassa", 1));
    doc.addOrUpdateKonto(BollDoc::Konto(5010, "Hyra", 3));

    doc.addVerifikat({0, "Ingående saldon", Date(0, 1, 1)});
    for (int i = 1; i <= 12; ++i) {
        BollDoc::Verifikat v{i, "Hyra", Date(2018, i, 1)};
        v.addRad({Date(2018, 12, 25), 1910, parsePengar("-8000")});
        v.addRad({Date(2018, 12, 25), 5010, parsePengar("8000")});
        doc.addVerifikat(std::move(v));
    }
    doc.addOrUpdateKonto({0, "Foo", 0});
    return doc;
}
} // namespace

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

TEST_CASE("Verifikat omslutning") {
    std::vector<std::vector<int64_t>> verifikat = {
        {100, 150, -250},
        {-100, -100, -150, 350},
        {-10, -15, -20, 12, 18, 7, 8},
        {1, 5, 7, -21},
    };
    BollDoc doc(2074, "Ruffel & Båg", "551122-1234", 2018, "SEK", false);

    int verifikatNum = 0;
    for (auto& v : verifikat) {
        BollDoc::Verifikat nyttVerifikat{verifikatNum++, "Abc",
                                         Date(2018, 1, 1)};
        for (auto& r : v) {
            nyttVerifikat.addRad({Date(2018, 12, 25), 1910, r});
        }
        doc.addVerifikat(std::move(nyttVerifikat));
    }
    Pengar omslutning;
    CHECK_UNARY(doc.getVerifikat(0).getOmslutning(omslutning));
    CHECK(omslutning.get() == 250);
    CHECK_UNARY(doc.getVerifikat(1).getOmslutning(omslutning));
    CHECK(omslutning.get() == 350);
    CHECK_UNARY(doc.getVerifikat(2).getOmslutning(omslutning));
    CHECK(omslutning.get() == 45);
    CHECK_FALSE(doc.getVerifikat(3).getOmslutning(omslutning));
    CHECK(omslutning.get() == -8);
}

TEST_CASE("Equality") {
    BollDoc ref = createDoc();
    BollDoc cmp = createDoc();
    CHECK(ref == cmp);
    cmp.addOrUpdateKonto({1, "Bar", 0});
    CHECK_FALSE(ref == cmp);
    cmp = createDoc();
    cmp.addVerifikat({13, "FooBar", Date(2018, 5, 1)});
    CHECK_FALSE(ref == cmp);
}

TEST_CASE("Update") {
    BollDoc doc = createDoc();

    doc.setVerifikatText(0, "Foo");
    CHECK(doc.getVerifikat(0).getText() == "Foo");
    CHECK_THROWS_WITH(doc.setVerifikatTransdatum(0, Date(2019, 01, 01)),
        "Wrong year in verifikat, document has year 2018 and verifikat has year 2019");
    doc.setVerifikatTransdatum(0, Date(2018, 03, 27));
    CHECK(doc.getVerifikat(0).getTransdatum() == Date(2018, 03, 27));
}

TEST_CASE("Add") {
    BollDoc doc = createDoc();

    BollDoc::Verifikat v{13, "Lån", Date(2019, 6, 1)};
    v.addRad({Date(2018, 12, 25), 1910, parsePengar("-8000")});
    v.addRad({Date(2018, 12, 25), 5010, parsePengar("8000")});
    CHECK_THROWS_WITH(doc.addVerifikat(std::move(v)),
        "Wrong year in verifikat, document has year 2018 and verifikat has year 2019");
    v.setTransdatum(Date(2018, 6, 1));
    doc.addVerifikat(std::move(v));
    CHECK(doc.getVerifikat(13).getTransdatum() == Date(2018, 6, 1));
}

TEST_CASE("New Year") {
    BollDoc doc = createDoc();
    BollDoc newYear = doc.newYear();
    CHECK(newYear.getBokforingsar() == 2019);
    CHECK(newYear.getVerifikationer().size() == 1);
    const BollDoc::Verifikat& v = newYear.getVerifikat(0);
    CHECK(v.getText() == "Ingående saldon");
    CHECK(v.getRader().size() == 1);
    const BollDoc::Rad& rad = v.getRad(0);
    CHECK(rad.getKonto() == 1910);
    CHECK(rad.getPengar() == parsePengar("-96000"));
}
