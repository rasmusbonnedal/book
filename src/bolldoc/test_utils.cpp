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

TEST_CASE("Checksum") {
    CHECK(Utils::calcChecksum("<bollbok>abcd1234</bollbok>") == 60033038);
    CHECK(Utils::calcChecksum("<bollbok xxx yyy zzz >abcd1234</bollbok>") == 60033038);
    CHECK(Utils::calcChecksum("<bollbok>abcd123</bollbok>") == 3975670578);
    CHECK(Utils::calcChecksum("<bollbok>abcd12345</bollbok>") == 1821926968);
    CHECK(Utils::calcChecksum(u8"<bollbok>z\u00df\u6c34\U0001f34c</bollbok>") == 3199064993);
}

TEST_CASE("Checksum fail") {
    CHECK_THROWS_WITH(Utils::calcChecksum("<bollbo></bollbok>"), "Could not calculate checksum");
    CHECK_THROWS_WITH(Utils::calcChecksum("<bollbok</bollbok>"), "Could not calculate checksum");
    CHECK_THROWS_WITH(Utils::calcChecksum("<bollbok></bollbok"), "Could not calculate checksum");
}
