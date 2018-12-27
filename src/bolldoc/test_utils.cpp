#include <iostream>

#include <doctest.h>

#include "utils.h"

TEST_CASE("1") {
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
