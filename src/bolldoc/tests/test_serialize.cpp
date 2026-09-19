#include <doctest.h>

#include "serialize.h"
#include "utils.h"

#include <fstream>
#include <sstream>

TEST_CASE("Serialize load") {
    std::ifstream input("../../../../docs/bok1.bollbok", std::ios_base::binary);
    REQUIRE(input.good());
    BollDoc doc = Serialize::loadDocument(input);
    CHECK(doc.getVersion() == 2074);
    CHECK(doc.getFirma() == "");
    CHECK(doc.getOrgnummer() == "");
    CHECK(doc.getBokforingsar() == 2018);
    CHECK(doc.getValuta() == "SEK");

    CHECK(doc.getKonto(1910).getText() == "Kassa");
    CHECK(doc.getKonto(2010).getTyp() == 1);
    CHECK(doc.getKonto(2350).getNormalt().empty());
    CHECK(doc.getKonto(5010).getNormalt() == "+");
    CHECK(doc.getKonto(2350).getTagg().empty());
    CHECK(doc.getKonto(3000).getTagg() == "F");
    CHECK_THROWS_WITH(doc.getKonto(9999), "Could not find konto 9999");

    CHECK(doc.getVerifikat(1).getText() == "Hyra");
    CHECK(doc.getVerifikat(2).getTransdatum() == Date(2018, 2, 1));
    CHECK_THROWS_WITH(doc.getVerifikat(4),
                      "Could not find verifikat 4");

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
            if (!r.getStruken())
                sum += r.getPengar().get();
        }
        CHECK(sum == 0);
    }
}

TEST_CASE("Serialize save") {
    BollDoc doc(2074, "Ruffel & Båg", "551122-1234", 2018, "SEK", true);
    doc.addOrUpdateKonto(BollDoc::Konto(1110, "Byggnader", 1));
    doc.addOrUpdateKonto(BollDoc::Konto(3000, "Försäljning inom Sverige", 3, "-", "F"));

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

    BollDoc::Verifikat v3{2, "Försäljning", parseDate("2018-02-01"), true};
    v3.addRad({parseDate("2018-12-25"), 1910, parsePengar("3000")});
    v3.addRad({parseDate("2018-12-25"), 3000, parsePengar("-3000"),
               parseDate("2019-07-21")});
    v3.addRad({parseDate("2019-07-21"), 3001, parsePengar("-3000")});
    doc.addVerifikat(std::move(v3));

    const std::string testfile = "../../../../docs/output.bollbok";

    std::ofstream output(testfile, std::ios_base::binary);
    REQUIRE(output.good());
    Serialize::saveDocumentCustom(doc, output);
    output.close();

    std::ifstream input(testfile, std::ios_base::binary);
    REQUIRE(input.good());
    auto loadedDoc = Serialize::loadDocument(input);
    REQUIRE(doc == loadedDoc);
    CHECK(loadedDoc.getVerifikat(BollDoc::BokforingsorderIdStart).isBokforingsorder());
    CHECK_FALSE(loadedDoc.getVerifikat(1).isBokforingsorder());
}

TEST_CASE("Separate ids and receipt associations survive saving after conversion") {
    BollDoc doc(2074, "Test", "", 2018, "SEK", false);
    for (int id = 0; id < 4; ++id) {
        BollDoc::Verifikat v{id, "Entry", Date(2018, 1, 1)};
        v.addRad({Date(2018, 1, 1), 1910, 100});
        doc.addVerifikat(std::move(v));
    }
    auto v = doc.getVerifikat(1);
    v.convertToBokforingsorder(Date(2018, 1, 1));
    doc.updateVerifikat(std::move(v));
    std::stringstream saved;
    Serialize::saveDocumentCustom(doc, saved);
    auto legacyXml = saved.str();
    const auto idPos = legacyXml.find("unid=\"1000000\"");
    REQUIRE(idPos != std::string::npos);
    legacyXml.replace(idPos, std::string("unid=\"1000000\"").size(), "unid=\"1\"");
    std::stringstream legacy(legacyXml);
    auto migrated = Serialize::loadDocument(legacy, true);
    REQUIRE(migrated.getVerifikationer().size() == doc.getVerifikationer().size());
    for (const auto& entry : doc.getVerifikationer())
        CHECK(migrated.getVerifikat(entry.getUnid()) == entry);
    auto loaded = Serialize::loadDocument(saved);
    REQUIRE(loaded.getVerifikationer().size() == doc.getVerifikationer().size());
    for (const auto& entry : doc.getVerifikationer())
        CHECK(loaded.getVerifikat(entry.getUnid()) == entry);
    CHECK(loaded.getNextVerifikatId() == 4);
    CHECK(loaded.getNextBokforingsorderId() == 1000001);
    CHECK(loaded.getVerifikat(1000000).getKvittoId() == 1);
    auto order = loaded.getVerifikat(1000000);
    order.promoteToVerifikat();
    loaded.updateVerifikat(std::move(order));
    std::stringstream promoted;
    Serialize::saveDocumentCustom(loaded, promoted);
    CHECK(Serialize::loadDocument(promoted) == loaded);
}

TEST_CASE("XML orders promoted verifikat by number and puts drafts last") {
    BollDoc doc(2074, "Test", "", 2018, "SEK", false);
    auto add = [&](int id, bool draft = false) {
        BollDoc::Verifikat v{id, "Entry", Date(2018, 1, 1), draft};
        v.addRad({Date(2018, 1, 1), 1910, 100});
        doc.addVerifikat(std::move(v));
    };
    add(0);
    add(183);
    add(1000000, true);
    add(1000001, true);
    add(184);
    auto order = doc.getVerifikat(1000000);
    order.promoteToVerifikat();
    REQUIRE(doc.updateVerifikat(std::move(order)) == 185);
    const auto original = doc;
    for (auto save : {Serialize::saveDocumentCustom, Serialize::saveDocument}) {
        std::stringstream xml;
        save(doc, xml);
        const auto text = xml.str();
        CHECK(text.find("unid=\"184\"") < text.find("unid=\"185\""));
        CHECK(text.find("unid=\"185\"") < text.find("unid=\"1000001\""));
        auto loaded = Serialize::loadDocument(xml);
        REQUIRE(loaded.getVerifikationer().size() == 5);
        const int expected[] = {0, 183, 184, 185, 1000001};
        for (size_t i = 0; i < 5; ++i) {
            CHECK(loaded.getVerifikationer()[i].getUnid() == expected[i]);
            CHECK(loaded.getVerifikat(expected[i]) == doc.getVerifikat(expected[i]));
        }
        CHECK(doc == original);
    }
}
