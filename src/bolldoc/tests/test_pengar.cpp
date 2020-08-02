#include <doctest.h>

#include "timer.h"
#include "pengar.h"

TEST_CASE("Parse Pengar") {
    CHECK(parsePengar("-1000,29").get() == -100029);
    CHECK(parsePengar("-1000,2").get() == -100020);
    CHECK(parsePengar("-1000.").get() == -100000);
    CHECK(parsePengar("-1000").get() == -100000);
    CHECK(parsePengar("1000.29").get() == 100029);
    CHECK(parsePengar("1000.2").get() == 100020);
    CHECK(parsePengar("1000.").get() == 100000);
    CHECK(parsePengar("1000").get() == 100000);
    CHECK(parsePengar("-8000000000.01").get() == -800000000001);
    CHECK(parsePengar("-0.92").get() == -92);
}

TEST_CASE("Parse fail") {
    CHECK_THROWS_WITH(parsePengar(".2"), "Could not parse .2 as number");
    CHECK_THROWS_WITH(parsePengar("a"), "Could not parse a as number");
    CHECK_THROWS_WITH(parsePengar("2.0b"), "Could not parse 2.0b as number");
}

TEST_CASE("Pengar Stream") {
    CHECK("-1 000,29" == toString2(Pengar(-100029)));
    CHECK("-1 000,20" == toString2(Pengar(-100020)));
    CHECK("-1 000" == toString2(Pengar(-100000)));
    CHECK("1 000,29" == toString2(Pengar(100029)));
    CHECK("1 000,20" == toString2(Pengar(100020)));
    CHECK("1 000" == toString2(Pengar(100000)));
    CHECK("-8 000 000 000,01" == toString2(Pengar(-800000000001)));
    CHECK("-100,20" == toString2(Pengar(-10020)));
    CHECK("-10,20" == toString2(Pengar(-1020)));
    CHECK("-1,20" == toString2(Pengar(-120)));
    CHECK("-0,20" == toString2(Pengar(-20)));
    CHECK("100,20" == toString2(Pengar(10020)));
    CHECK("10,20" == toString2(Pengar(1020)));
    CHECK("1,20" == toString2(Pengar(120)));
    CHECK("0,20" == toString2(Pengar(20)));
}

TEST_CASE("Pengar equality") {
    CHECK(parsePengar("-123,45") == Pengar(-12345));
    CHECK(parsePengar("123.45") == Pengar(12345));
    CHECK_FALSE(parsePengar("123.45") == Pengar(-12345));
}

TEST_CASE("Pengar arithmetic") {
    CHECK(-Pengar(1000232) == Pengar(-1000232));
    CHECK(Pengar(12) == -Pengar(-12));
    CHECK(Pengar(-1) == -Pengar(1));
    CHECK(Pengar(123) + Pengar(321) == Pengar(444));
    CHECK(Pengar(321) - Pengar(123) == Pengar(198));
}

TEST_CASE("Parse Pengar timer") {
    Pengar sum;
    Stopwatch t;
    t.start();
    for (int i = 0; i < 1000; ++i) {
        sum += parsePengar("-1000000.01");
    }
    float time = t.stop();
    Pengar ref = parsePengar("-1000000010");
    CHECK((sum == ref));
    CHECK(time < 0.1f);
}
