#include "moms.h"

#include <nlohmann/json.hpp>

#include <fstream>
#include <iostream>
#include <set>

namespace {
void check_date(DateType date) {
    if (date < DATETYPE_Q1 || date > DATETYPE_DECEMBER) {
        throw std::runtime_error("Illegal month number " + std::string(dateTypeToString(date)));
    }
}

int round_skv(const Pengar& p) {
    return (int)(p.get() / 100);
}

int parseInt(const std::string& s) {
    if (s.length() == 0) return 0;
    return std::stol(s);
}

bool is_balance_acct(int konto) {
    int first_digit = konto / 1000;
    return first_digit == 1 || first_digit == 2;
}

bool is_purchase_acct(int konto) {
    int first_digit = konto / 1000;
    return first_digit == 4 || first_digit == 5 || first_digit == 6;
}

}  // namespace

FieldSaldo summarize_moms(const BollDoc& doc, DateType date, const KontoMap& konto_map, Verifikat& redovisning) {
    check_date(date);

    DateRange date_range = dateTypeToRange(date, doc.getBokforingsar());

    std::vector<const BollDoc::Verifikat*> verifikat;
    bool is_locked = false;
    for (const auto& v : doc.getVerifikationer()) {
        if (date_range.isInRange(v.getTransdatum())) {
            if (v.getText().find("Momsredovisning") != std::string::npos) {
                is_locked = true;            
            } else {
                verifikat.push_back(&v);
            }
        }
    }

    FieldSaldo fields;
    for (const auto& [field, konton] : konto_map) {
        Pengar saldo;
        bool is_balance = false;
        for (const auto& konto : konton) {
            for (const auto v : verifikat) {
                for (const auto& rad : v->getRader()) {
                    if (rad.getKonto() == konto && !rad.getStruken()) {
                        if (is_purchase_acct(konto)) {
                            saldo += rad.getPengar();
                        } else {
                            saldo += -rad.getPengar();
                        }
                        if (is_balance_acct(konto)) {
                            redovisning[konto] += -rad.getPengar();
                            is_balance = true;
                        }
                    }
                }
            }
        }
        if (saldo != 0) {
            if (is_balance) {
                fields[49] += Pengar(round_skv(saldo) * 100);
            }
            fields[field] = Pengar(round_skv(saldo) * 100);
        }
    }
    // Fix sign on avdragen moms
    fields[48] = -fields[48];
    if (fields[49].get() < 0) {
        redovisning[1650] = -fields[49];
    } else {
        redovisning[2650] = -fields[49];
    }
    Pengar oresavrundning;
    for (const auto& [konto, rad]: redovisning) {
        oresavrundning += rad;
    }
    if (abs(oresavrundning.get()) >= 1000) {
        std::cout << "Öresavrundning curiously large: " + to_string(oresavrundning);
    }
    if (oresavrundning != 0) {
        redovisning[3740] = -oresavrundning;
    }
    return fields;
}

std::string gen_moms_eskd(const BollDoc& doc, DateType date_type, const FieldSaldo& field_saldo, const FieldToSkv& field_to_skv) {
    check_date(date_type);

    if (field_saldo.count(49) == 0) {
        throw std::runtime_error("field_saldo lacks sum field");
    }

    std::string outxml = R"(<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE eSKDUpload PUBLIC "-//Skatteverket, Sweden//DTD Skatteverket eSKDUpload-DTD Version 6.0//SV" "https://www.skatteverket.se/download/18.3f4496fd14864cc5ac99cb1/1415022101213/eSKDUpload_6p0.dtd">
<eSKDUpload Version="6.0">
)";
    outxml += "  <OrgNr>" + doc.getOrgnummer() + "</OrgNr>\n";
    outxml += "  <Moms>\n";
    char buf[12];

    snprintf(buf, 11, "%04d%02d", doc.getBokforingsar(), dateTypeToRange(date_type, doc.getBokforingsar()).getEnd().getMonth());
    outxml += "    <Period>" + std::string(buf) + "</Period>\n";
    for (const auto& [field, saldo] : field_saldo) {
        auto it = field_to_skv.find(field);
        if (it == field_to_skv.end()) {
            throw std::runtime_error("Field " + std::to_string(field) + " not in field_to_skv map");        
        }
        const std::string& skvxml = it->second;
        int value = round_skv(saldo);
        if (field != 49 && value < 0) {
            throw std::runtime_error("Unexpected negative value in eSKD " + std::to_string(field) + " = " + std::to_string(value));
        }
        outxml += "    <" + skvxml + ">" + std::to_string(value) + "</" + skvxml + ">\n";
    }
    outxml += "  </Moms>\n";
    outxml += "</eSKDUpload>\n";
    return outxml;
}

void loadMomsMapping(KontoMap& konto_map, FieldToSkv& field_to_skv, const std::string& filename) {
    std::ifstream ifs(filename);
    if (ifs.is_open()) {
        auto j = nlohmann::json::parse(ifs);
        for (auto it = j.begin(); it != j.end(); ++it) {
            for (auto it2 = it.value().begin(); it2 != it.value().end(); ++it2) {
                int skv_no = parseInt(it2.key());
                field_to_skv[skv_no] = it2.value()["skvxml"];
                for (int acct : it2.value()["accts"]) {
                    konto_map[skv_no].push_back(acct);
                }
            }
        }
    } else {
        throw std::runtime_error("Could not open moms json file " + filename);
    }
}
