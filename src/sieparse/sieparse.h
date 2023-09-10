#pragma once

#include <istream>
#include <map>
#include <string>
#include <vector>

// Alla saldon �r i �ren (12345 = 123,45 kr)
// Alla str�ngar �r i UTF-8

class SIEKonto {
   public:
    SIEKonto() : id(0), sru(0) {}
    int id;
    std::string text;
    int sru;
};

class SIEBalansResultat {
   public:
    std::map<int, int64_t> ib;
    std::map<int, int64_t> ub;
    std::map<int, int64_t> resultat;
};

class SIETransaktion {
   public:
       enum TransaktionType {
           NORMAL,
           RTRANS,
           BTRANS
    };

    int64_t konto;
    int64_t saldo;
    TransaktionType typ;
};

class SIEVerifikat {
   public:
    SIEVerifikat() : id(0), transaktionsdatum(0), bokforingsdatum(0) {}
    int64_t id;
    int64_t transaktionsdatum;
    std::string text;
    int64_t bokforingsdatum;
    std::vector<SIETransaktion> rader;
};

class SIEData {
   public:
    std::map<std::string, std::vector<std::string>> fields;
    std::map<int, SIEBalansResultat> balans_resultat;
    std::vector<SIEVerifikat> verifikat;
    std::map<int, SIEKonto> kontoplan;
};

bool parse(SIEData& siedata, std::istream& is);
