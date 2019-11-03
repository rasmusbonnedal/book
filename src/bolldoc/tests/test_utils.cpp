#include <iostream>

#include <doctest.h>

#include "utils.h"

TEST_CASE("Checksum") {
    CHECK(Utils::calcChecksum("<bollbok>abcd1234</bollbok>") == 60033038);
    CHECK(Utils::calcChecksum("<bollbok xxx yyy zzz >abcd1234</bollbok>") ==
          60033038);
    CHECK(Utils::calcChecksum("<bollbok>abcd123</bollbok>") == 3975670578);
    CHECK(Utils::calcChecksum("<bollbok>abcd12345</bollbok>") == 1821926968);
    CHECK(Utils::calcChecksum(u8"<bollbok>z\u00df\u6c34\U0001f34c</bollbok>") ==
          3199064993);
}

TEST_CASE("Checksum fail") {
    CHECK_THROWS_WITH(Utils::calcChecksum("<bollbo></bollbok>"),
                      "Could not calculate checksum");
    CHECK_THROWS_WITH(Utils::calcChecksum("<bollbok</bollbok>"),
                      "Could not calculate checksum");
    CHECK_THROWS_WITH(Utils::calcChecksum("<bollbok></bollbok"),
                      "Could not calculate checksum");
}
