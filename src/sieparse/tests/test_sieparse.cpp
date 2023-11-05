#include <doctest.h>

#include <iostream>
#include <sstream>

#include "parsenumber.h"
#include "sieparse.h"
#include "siewrite.h"
#include "string_utils.h"

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
    "#KONTO 1011 \"Balanserade utgifter f\x94r forskning och utveckling\"\n"
    "#SRU 1011 7201\n"
    "#IB 0 1385 10000.40\n"
    "#IB -1 1510 231866.00\n"
    "#IB 0 1510 90909.20\n"
    "#IB 0 1522 -100909.60\n"
    "#UB -1 1385 10000.00\n"
    "#UB 0 1385 91000.00\n"
    "#UB -2 1510 231866.00\n"
    "#UB -1 1510 90909.00\n"
    "#UB 0 1510 0.39\n"
    "#RES -1 3011 -1053365.54\n"
    "#RES 0 3011 -91000.39\n"
    "#VER \"V\" \"1\" 20230101 \"Bankavgift\" 20230103\n"
    "{\n"
    "\t#TRANS 1933 {} -70.00\n"
    "\t#TRANS 6570 {} 70.00\n"
    "}\n";

TEST_CASE("SIE parsing") {
    std::stringstream ss(sie_testdata);
    SIEData siedata;
    CHECK(parse(siedata, ss));

    CHECK(siedata.foretags_namn == "Myrkotten Konsult AB");
    CHECK(siedata.org_nummer == "559290-0004");
    CHECK(siedata.rakenskapsar_start == 20230101);
    CHECK(siedata.rakenskapsar_slut == 20231231);

    // Check #KONTO
    CHECK(siedata.kontoplan.size() == 2);
    CHECK(siedata.kontoplan[1010].id == 1010);
    CHECK(siedata.kontoplan[1010].text == "Utvecklingsutgifter");
    CHECK(siedata.kontoplan[1010].sru == 7201);
    CHECK(siedata.kontoplan[1011].id == 1011);
    CHECK(siedata.kontoplan[1011].text == "Balanserade utgifter f\xc3\xb6r forskning och utveckling");
    CHECK(siedata.kontoplan[1011].sru == 7201);

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

const char* siewrite_testdata =
    "#FLAGGA 0\n"
    "#PROGRAM \"Bollbok Imgui\" 1.0\n"
    "#FORMAT PC8\n"
    "#GEN 20230924\n"
    "#SIETYP 4\n"
    "#ORGNR 5592900004\n"
    "#FNAMN \"Myrkotten Konsult AB\"\n"
    "#RAR 0 20230101 20231231\n"
    "#KPTYP BAS2014\n"
    "#KONTO 1010 \"Utvecklingsutgifter\"\n"
    "#SRU 1010 7201\n"
    "#KONTO 1011 \"Balanserade utgifter f\x94r forskning och utveckling\"\n"
    "#SRU 1011 7201\n"
    "#IB 0 1385 10000.40\n"
    "#IB 0 1510 90909.20\n"
    "#IB 0 1522 -100909.60\n"
    "#UB 0 1385 91000.00\n"
    "#UB 0 1510 0.39\n"
    "#RES 0 3011 -91000.39\n"
    "#VER \"V\" \"1\" 20230101 \"Bankavgift\" 20230103\n"
    "{\n"
    "\t#TRANS 1933 {} -70.00\n"
    "\t#TRANS 6570 {} 70.00\n"
    "}\n";

TEST_CASE("SIE write") {
    SIEData siedata;
    siedata.foretags_namn = "Myrkotten Konsult AB";
    siedata.org_nummer = "559290-0004";
    siedata.rakenskapsar_start = 20230101;
    siedata.rakenskapsar_slut = 20231231;

    siedata.kontoplan[1010] = SIEKonto(1010, "Utvecklingsutgifter", 7201);
    siedata.kontoplan[1011] = SIEKonto(1011, "Balanserade utgifter f\xc3\xb6r forskning och utveckling", 7201);

    auto& br = siedata.balans_resultat[0];
    br.ib[1385] = 1000040;
    br.ib[1510] = 9090920;
    br.ib[1522] = -10090960;
    br.ub[1385] = 9100000;
    br.ub[1510] = 39;
    br.resultat[3011] = -9100039;

    SIEVerifikat v;
    v.transaktionsdatum = 20230101;
    v.id = 1;
    v.text = "Bankavgift";
    v.bokforingsdatum = 20230103;
    v.rader.emplace_back(1933, -7000, SIETransaktion::NORMAL);
    v.rader.emplace_back(6570, 7000, SIETransaktion::NORMAL);
    siedata.verifikat.push_back(v);

    std::stringstream ss;
    REQUIRE(siewrite(siedata, 20230924, ss));

    std::string s1 = ss.str();
    std::string s2 = siewrite_testdata;
    for (int i = 0; i < std::min(s1.size(), s2.size()); ++i) {
        if (s1[i] != s2[i]) {
            std::cout << s1[i] << " (" << int(s1[i]) << ") != " << s2[i] << " (" << int(s2[i]) << ") at " << i << std::endl;
        }
    }
    CHECK(ss.str() == std::string(siewrite_testdata));
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
    CHECK((parse_number("00", n) && (n == 0)));

    CHECK_FALSE(parse_number("", n));
    CHECK_FALSE(parse_number("-", n));
    CHECK_FALSE(parse_number("5-", n));
}

TEST_CASE("UTF-8") {
    CHECK(convert_cp437_to_utf8("\x94") == "\xc3\xb6");
}

TEST_CASE("cp437") {
    // c3 b6 = ö
    CHECK(convert_utf8_to_cp437("\xc3\xb6") == "\x94");
    CHECK(convert_utf8_to_cp437("abc123") == "abc123");
}
