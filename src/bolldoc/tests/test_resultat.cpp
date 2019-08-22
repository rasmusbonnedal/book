#include <doctest.h>

#include "bolldoc.h"
#include "resultat.h"

namespace {
    BollDoc createDoc() {
        BollDoc doc(2074, "Ruffel & Båg", "551122-1234", 2018, "SEK", false);

        doc.addVerifikat({0, "Ingående saldon", Date(0, 1, 1)});
        for (int i = 1; i <= 1000; ++i) {
            BollDoc::Verifikat v{i, "", Date(2018, 1, 1)};
            v.addRad({Date(2018, 12, 25), 1910, Pengar(-1000)});
            v.addRad({Date(2018, 12, 25), 5010, Pengar(1000)});
            doc.addVerifikat(std::move(v));
        }
        return doc;
    }
}

TEST_CASE("Resultat") {
    BollDoc doc = createDoc();
    std::vector<Resultat> resultat = calcResultat(doc.getVerifikatRange(0, 1001));
    REQUIRE(resultat.size() == 2);
    CHECK(resultat[0].getUnid() == 1910);
    CHECK(resultat[0].getPengar().get() == -1000000);
    CHECK(resultat[1].getUnid() == 5010);
    CHECK(resultat[1].getPengar().get() == 1000000);
}
