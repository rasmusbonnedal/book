#pragma once

#include <istream>
#include <map>
#include <string>
#include <vector>

// Alla saldon är i ören (12345 = 123,45 kr)
// Alla strängar är i UTF-8

class SIEKonto {
   public:
    SIEKonto() : id(0), sru(-1) {}
    SIEKonto(int _id, const std::string& _text, int _sru) : id(_id), text(_text), sru(_sru) {}
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
    enum TransaktionType { NORMAL, RTRANS, BTRANS };
    SIETransaktion(int64_t _konto, int64_t _saldo, TransaktionType _typ) : konto(_konto), saldo(_saldo), typ(_typ) {}

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
    SIEData() : rakenskapsar_start(0), rakenskapsar_slut(0) {}
    std::map<std::string, std::vector<std::string>> fields;
    std::map<int, SIEBalansResultat> balans_resultat;
    std::vector<SIEVerifikat> verifikat;
    std::map<int, SIEKonto> kontoplan;
    int rakenskapsar_start, rakenskapsar_slut;
    std::string foretags_namn;
    std::string org_nummer;
};

bool parse(SIEData& siedata, std::istream& is);
