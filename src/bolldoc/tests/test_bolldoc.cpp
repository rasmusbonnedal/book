#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>

#include "bolldoc.h"

TEST_CASE("Setup") {
    BollDoc doc(2074, "Ruffel & Båg", "551122-1234", 2018, "SEK", false);
    CHECK(doc.getVersion() == 2074);
    CHECK(doc.getFirma() == "Ruffel & Båg");
    CHECK(doc.getOrgnummer() == "551122-1234");
    CHECK(doc.getBokforingsar() == 2018);
    CHECK(doc.getValuta() == "SEK");
    CHECK(doc.getAvslutat() == false);
}
