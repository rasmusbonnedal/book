#include "moms.h"

#include <set>

namespace {
void check_month(int month) {
    if (month < 1 || month > 12) {
        throw std::runtime_error("Illegal month number " + std::to_string(month));
    }
}

int round_skv(const Pengar& p) {
    return (int)(p.get() / 100);
}

}  // namespace

FieldSaldo summarize_moms(const BollDoc& doc, int month, const KontoMap& konto_map, Verifikat& redovisning) {
    const std::set<int> add_fields{10, 11, 12, 30, 31, 32, 48, 60, 61, 62};
    check_month(month);

    std::vector<const BollDoc::Verifikat*> verifikat;
    bool is_locked = false;
    for (const auto& v : doc.getVerifikationer()) {
        if (v.getTransdatum().getMonth() == month) {
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
        for (const auto& konto : konton) {
            for (const auto v : verifikat) {
                for (const auto& rad : v->getRader()) {
                    if (rad.getKonto() == konto && !rad.getStruken()) {
                        saldo += rad.getPengar();
                        if (add_fields.count(field)) {
                            redovisning[konto] += -rad.getPengar();
                        }
                    }
                }
            }
        }
        if (saldo != 0) {
            fields[field] = saldo;
        }
    }
    sum_moms(fields);
    if (fields[49].get() > 0) {
        redovisning[1650] = fields[49];
    } else {
        redovisning[2650] = fields[49];
    }
    Pengar oresavrundning;
    for (const auto& [konto, rad]: redovisning) {
        oresavrundning += rad;
    }
    if (oresavrundning.get() >= 100 || oresavrundning.get() <= -100) {
        throw std::runtime_error("Momsredovisning doesn't add up: " + to_string(oresavrundning));
    }
    if (oresavrundning != 0) {
        redovisning[3740] = -oresavrundning;
    }
    return fields;
}

void sum_moms(FieldSaldo& field_saldo) {
    const std::set<int> add_fields{10, 11, 12, 30, 31, 32, 48, 60, 61, 62};

    int field_49 = 0;
    for (const auto& [field, saldo] : field_saldo) {
        if (add_fields.count(field)) {
            field_49 += round_skv(saldo);
        }
    }
    field_saldo[49] = Pengar(field_49 * 100);
}

std::string gen_moms_eskd(const BollDoc& doc, int month, const FieldSaldo& field_saldo, const FieldToSkv& field_to_skv) {
    check_month(month);

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
    snprintf(buf, 11, "%04d%02d", doc.getBokforingsar(), month);
    outxml += "    <Period>" + std::string(buf) + "</Period>\n";
    for (const auto& [field, saldo] : field_saldo) {
        auto it = field_to_skv.find(field);
        if (it == field_to_skv.end()) {
            throw std::runtime_error("Field " + std::to_string(field) + " not in field_to_skv map");        
        }
        const std::string& skvxml = it->second;
        int value = -round_skv(saldo);
        if (field == 48) {
            value = -value;
        }
        outxml += "    <" + skvxml + ">" + std::to_string(value) + "</" + skvxml + ">\n";
    }
    outxml += "  </Moms>\n";
    outxml += "</eSKDUpload>\n";
    return outxml;
}
