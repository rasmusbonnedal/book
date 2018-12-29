#include <iostream>

#include <doctest.h>

#include "utils.h"

TEST_CASE("Parse Pengar") {
    CHECK(Utils::parsePengar("-1000.29") == -100029);
    CHECK(Utils::parsePengar("-1000.2") == -100020);
    CHECK(Utils::parsePengar("-1000.") == -100000);
    CHECK(Utils::parsePengar("-1000") == -100000);
    CHECK(Utils::parsePengar("1000.29") == 100029);
    CHECK(Utils::parsePengar("1000.2") == 100020);
    CHECK(Utils::parsePengar("1000.") == 100000);
    CHECK(Utils::parsePengar("1000") == 100000);
    CHECK(Utils::parsePengar("-8000000000.01") == -800000000001);
}

TEST_CASE("Parse fail") {
    CHECK_THROWS_WITH(Utils::parsePengar(".2"), "Could not parse .2 as number");
    CHECK_THROWS_WITH(Utils::parsePengar("a"), "Could not parse a as number");
    CHECK_THROWS_WITH(Utils::parsePengar("2.0b"), "Could not parse 2.0b as number");
}
