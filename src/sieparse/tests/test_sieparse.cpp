#include <doctest.h>

#include <sstream>

#include "sieparse.h"
#include "parsenumber.h"

const char* sie_testdata =
    "#FLAGGA 0\n"
    "#PROGRAM \"Bokio\" 1.0\n"
    "#FORMAT PC8\n"
    "#GEN 20230901\n"
    "#SIETYP 4\n"
    "#ORGNR 5592900004\n"
    "#FNAMN \"Myrkotten Konsult AB\"\n"
    "#RAR 0 20230101 20231231\n"
    "#RAR -1 20220101 20221231\n"
    "#RAR -2 20201214 20211231\n"
    "#KPTYP BAS2014\n"
    "#KONTO 1010 \"Utvecklingsutgifter\"\n"
    "#SRU 1010 7201\n"
    "#IB 0 1385 10000.00\n"
    "#IB -1 1510 231866.00\n"
    "#IB 0 1510 90909.00\n"
    "#UB -1 1385 10000.00\n"
    "#UB 0 1385 91000.00\n"
    "#UB -2 1510 231866.00\n"
    "#UB -1 1510 90909.00\n"
    "#UB 0 1510 0.00\n"
    "#RES -1 3011 - 1053365.54\n"
    "#RES 0 3011 -712620.71\n"
    "#VER \"V\" \"1\" 20230101 \"Bankavgift\" 20230103\n"
    "{\n"
    "\t#TRANS 1933 {} -70.00\n"
    "\t#TRANS 6570 {} 70.00\n"
    "}\n";

TEST_CASE("SIE parsing") {
    std::stringstream ss(sie_testdata);
    SIEData siedata;
    CHECK(parse(siedata, ss));

    CHECK(siedata.fields["#ORGNR"][0] == "5592900004");
    CHECK(siedata.fields["#FNAMN"][0] == "Myrkotten Konsult AB");

    // Check #KONTO
    CHECK(siedata.kontoplan.size() == 1);
    CHECK(siedata.kontoplan[1010].id == 1010);
    CHECK(siedata.kontoplan[1010].text == "Utvecklingsutgifter");

    // Check #VER
    CHECK(siedata.verifikat.size() == 1);
    const auto& v = siedata.verifikat[0];
    CHECK(v.transaktionsdatum == 20230101);
    CHECK(v.id == 1);
    CHECK(v.text == "Bankavgift");
    CHECK(v.bokforingsdatum == 20230103);
    CHECK(v.rader.size() == 2);

    // Check #TRANS
    CHECK(v.rader[0].typ == SIETransaktion::NORMAL);
    CHECK(v.rader[0].konto == 1933);
    CHECK(v.rader[0].saldo == -7000);
    CHECK(v.rader[1].typ == SIETransaktion::NORMAL);
    CHECK(v.rader[1].konto == 6570);
    CHECK(v.rader[1].saldo == 7000);
}

TEST_CASE("Saldo parsing") {
    int64_t s;
    CHECK((parse_saldo("1234567", s) && (s == 123456700)));
    CHECK((parse_saldo("-1234567", s) && (s == -123456700)));
    CHECK((parse_saldo("1234567.1", s) && (s == 123456710)));
    CHECK((parse_saldo("-1234567.1", s) && (s == -123456710)));
    CHECK((parse_saldo("123.45", s) && (s == 12345)));
    CHECK((parse_saldo("-123.45", s) && (s == -12345)));

    CHECK_FALSE(parse_saldo("2.", s));
    CHECK_FALSE(parse_saldo("1.234", s));
    CHECK_FALSE(parse_saldo(".2", s));
    CHECK_FALSE(parse_saldo(".", s));
    CHECK_FALSE(parse_saldo("-", s));
    CHECK_FALSE(parse_saldo("-.", s));
    CHECK_FALSE(parse_saldo("-0", s));
}

TEST_CASE("Number parsing") {
    int64_t n;
    CHECK((parse_number("1234567", n) && (n == 1234567)));
    CHECK((parse_number("-1234567", n) && (n == -1234567)));

    CHECK_FALSE(parse_number("", n));
    CHECK_FALSE(parse_number("-", n));
    CHECK_FALSE(parse_number("5-", n));
}
