#include <doctest.h>

#include "timer.h"
#include "pengar.h"

TEST_CASE("Parse Pengar") {
    CHECK(parsePengar("-1000.29").get() == -100029);
    CHECK(parsePengar("-1000.2").get() == -100020);
    CHECK(parsePengar("-1000.").get() == -100000);
    CHECK(parsePengar("-1000").get() == -100000);
    CHECK(parsePengar("1000.29").get() == 100029);
    CHECK(parsePengar("1000.2").get() == 100020);
    CHECK(parsePengar("1000.").get() == 100000);
    CHECK(parsePengar("1000").get() == 100000);
    CHECK(parsePengar("-8000000000.01").get() == -800000000001);
}

TEST_CASE("Parse fail") {
    CHECK_THROWS_WITH(parsePengar(".2"), "Could not parse .2 as number");
    CHECK_THROWS_WITH(parsePengar("a"), "Could not parse a as number");
    CHECK_THROWS_WITH(parsePengar("2.0b"), "Could not parse 2.0b as number");
}

TEST_CASE("Pengar Stream") {
    CHECK("-1000.29" == toString(Pengar(-100029)));
    CHECK("-1000.20" == toString(Pengar(-100020)));
    CHECK("-1000" == toString(Pengar(-100000)));
    CHECK("1000.29" == toString(Pengar(100029)));
    CHECK("1000.20" == toString(Pengar(100020)));
    CHECK("1000" == toString(Pengar(100000)));
    CHECK("-8000000000.01" == toString(Pengar(-800000000001)));
}

TEST_CASE("Pengar equality") {
    bool b = parsePengar("-123.45") == Pengar(-12345);
    CHECK(b);
    b = parsePengar("123.45") == Pengar(12345);
    CHECK(b);
    b = parsePengar("123.45") == Pengar(-12345);
    CHECK_FALSE(b);
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
