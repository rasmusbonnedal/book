#include <doctest.h>

#include "date.h"

TEST_CASE("Date Parsing") {
    CHECK(parseDate("1776-07-04") == Date(1776, 7, 4));
    CHECK_EQ(parseDate("1836-02-23"), Date(1836, 2, 23));
}

TEST_CASE("Date ==") {
    CHECK(Date(2001, 10, 1) == Date(2001, 10, 1));
    CHECK(Date(2001, 10, 1) != Date(2001, 10, 2));
    CHECK(Date(2001, 10, 1) != Date(2001, 11, 1));
    CHECK(Date(2001, 10, 1) != Date(2002, 10, 1));
}

TEST_CASE("Date <") {
    CHECK(Date(2001, 10, 2) < Date(2002, 9, 1));
    CHECK(Date(2001, 10, 2) < Date(2001, 11, 1));
    CHECK(Date(2001, 10, 2) < Date(2001, 10, 3));
}

TEST_CASE("Valid Date") {
    CHECK_NOTHROW(Date(0, 1, 1));
    CHECK_NOTHROW(Date(1987, 12, 1));
    CHECK_NOTHROW(Date(1987, 1, 31));
    CHECK_NOTHROW(Date(1987, 3, 31));
    CHECK_NOTHROW(Date(1987, 4, 30));
    CHECK_NOTHROW(Date(1987, 5, 31));
    CHECK_NOTHROW(Date(1987, 6, 30));
    CHECK_NOTHROW(Date(1987, 7, 31));
    CHECK_NOTHROW(Date(1987, 8, 31));
    CHECK_NOTHROW(Date(1987, 9, 30));
    CHECK_NOTHROW(Date(1987, 10, 31));
    CHECK_NOTHROW(Date(1987, 11, 30));
    CHECK_NOTHROW(Date(1987, 12, 31));
    CHECK_NOTHROW(Date(1987, 2, 28));
    CHECK_NOTHROW(Date(2000, 2, 29));
    CHECK_NOTHROW(Date(2004, 2, 29));
    CHECK_NOTHROW(Date(1900, 2, 28));
}

TEST_CASE("Invalid Date") {
    CHECK_THROWS_WITH(Date(1987, 0, 1), "Invalid date 1987-00-01");
    CHECK_THROWS_WITH(Date(1987, 13, 1), "Invalid date 1987-13-01");
    CHECK_THROWS_WITH(Date(1987, 1, 0), "Invalid date 1987-01-00");
    CHECK_THROWS_WITH(Date(-1, 0, 1), "Invalid date 00-1-00-01");
    CHECK_THROWS_WITH(Date(1987, 1, 32), "Invalid date 1987-01-32");
    CHECK_THROWS_WITH(Date(1987, 3, 32), "Invalid date 1987-03-32");
    CHECK_THROWS_WITH(Date(1987, 4, 31), "Invalid date 1987-04-31");
    CHECK_THROWS_WITH(Date(1987, 5, 32), "Invalid date 1987-05-32");
    CHECK_THROWS_WITH(Date(1987, 6, 31), "Invalid date 1987-06-31");
    CHECK_THROWS_WITH(Date(1987, 7, 32), "Invalid date 1987-07-32");
    CHECK_THROWS_WITH(Date(1987, 8, 32), "Invalid date 1987-08-32");
    CHECK_THROWS_WITH(Date(1987, 9, 31), "Invalid date 1987-09-31");
    CHECK_THROWS_WITH(Date(1987, 10, 32), "Invalid date 1987-10-32");
    CHECK_THROWS_WITH(Date(1987, 11, 31), "Invalid date 1987-11-31");
    CHECK_THROWS_WITH(Date(1987, 12, 32), "Invalid date 1987-12-32");
    CHECK_THROWS_WITH(Date(1987, 2, 29), "Invalid date 1987-02-29");
    CHECK_THROWS_WITH(Date(2000, 2, 30), "Invalid date 2000-02-30");
    CHECK_THROWS_WITH(Date(2004, 2, 30), "Invalid date 2004-02-30");
    CHECK_THROWS_WITH(Date(1900, 2, 29), "Invalid date 1900-02-29");
}

TEST_CASE("Date parse fail") {
    CHECK_THROWS_WITH(parseDate("230-10-10"), "Could not parse 230-10-10 as a date");
}

TEST_CASE("Last day of month") {
    for (int i : { 1, 3, 5, 7, 8, 10, 12 }) {
        CHECK(lastDayOfMonth(Date(1999, i, 1)) == Date(1999, i, 31));
    }
    for (int i : { 4, 6, 9, 11 }) {
        CHECK(lastDayOfMonth(Date(1999, i, 1)) == Date(1999, i, 30));
    }
    CHECK(lastDayOfMonth(Date(1999, 2, 1)) == Date(1999, 2, 28));
    CHECK(lastDayOfMonth(Date(2000, 2, 1)) == Date(2000, 2, 29));
}
