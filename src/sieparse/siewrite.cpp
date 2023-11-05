#include "siewrite.h"

#include "string_utils.h"

#include <assert.h>

#include <chrono>
#include <iomanip>

namespace {
class Element {
   public:
    static Element String(const std::string& s) {
        Element elem;
        elem.m_type = ELEM_TYPE_STRING;
        elem.m_s = s;
        return elem;
    }
    static Element Token(const std::string& s) {
        Element elem;
        elem.m_type = ELEM_TYPE_TOKEN;
        elem.m_s = s;
        return elem;
    }
    static Element Int(int64_t i) {
        Element elem;
        elem.m_type = ELEM_TYPE_INT;
        elem.m_i = i;
        return elem;
    }
    static Element Valuta(int64_t v) {
        Element elem;
        elem.m_type = ELEM_TYPE_VALUTA;
        elem.m_i = v;
        return elem;
    }

    void write(std::ostream& os) const {
        switch (m_type) {
            case ELEM_TYPE_STRING:
                os << "\"" << convert_utf8_to_cp437(m_s) << "\"";
                break;
            case ELEM_TYPE_TOKEN:
                os << convert_utf8_to_cp437(m_s);
                break;
            case ELEM_TYPE_INT:
                os << m_i;
                break;
            case ELEM_TYPE_VALUTA:
                os.fill('0');
                os << (m_i / 100) << "." << std::setw(2) << (abs(m_i) % 100) << std::setw(0);
                break;
            default:
                assert(false);
        }
    }

   private:
    Element(){};
    enum ElementType { ELEM_TYPE_STRING, ELEM_TYPE_TOKEN, ELEM_TYPE_INT, ELEM_TYPE_VALUTA };
    std::string m_s;
    int64_t m_i;
    ElementType m_type;
};

void writetag(const std::string& tag, const std::vector<Element>& elements, std::ostream& os) {
    os << "#" << tag;
    for (const auto& elem : elements) {
        os << " ";
        elem.write(os);
    }
    os << std::endl;
}

void writerad(const SIETransaktion& rad, std::ostream& os) {
    std::string typ;
    switch (rad.typ) {
        case SIETransaktion::NORMAL:
            typ = "TRANS";
            break;
        case SIETransaktion::RTRANS:
            typ = "RTRANS";
            break;
        case SIETransaktion::BTRANS:
            typ = "BTRANS";
            break;
        default:
            return;
    }
    os << "\t";
    writetag(typ, {Element::Int(rad.konto), Element::Token("{}"), Element::Valuta(rad.saldo)}, os);
}

int64_t today() {
    auto t = std::chrono::system_clock::now();
    std::time_t tt = std::chrono::system_clock::to_time_t(t);
    auto tm = std::tm{0};
#if defined(_WIN32)
    gmtime_s(&tm, &tt);
#else
    gmtime_r(&tt, &tm);
#endif
    return (tm.tm_year + 1900) * 10000 + (tm.tm_mon + 1) * 100 + tm.tm_mday;
}



}  // namespace

bool siewrite(SIEData& siedata, int64_t today, std::ostream& os) {
    writetag("FLAGGA", {Element::Int(0)}, os);
    writetag("PROGRAM", {Element::String("Bollbok Imgui"), Element::Token("1.0")}, os);
    writetag("FORMAT", {Element::Token("PC8")}, os);
    writetag("GEN", {Element::Int(today)}, os);
    writetag("SIETYP", {Element::Int(4)}, os);
    writetag("ORGNR", {Element::Token(siedata.org_nummer.substr(0, 6) + siedata.org_nummer.substr(7, 4))}, os);
    writetag("FNAMN", {Element::String(siedata.foretags_namn)}, os);
    writetag("RAR", {Element::Int(0), Element::Int(siedata.rakenskapsar_start), Element::Int(siedata.rakenskapsar_slut)}, os);
    writetag("KPTYP", {Element::Token("BAS2014")}, os);
    for (const auto& [id, konto] : siedata.kontoplan) {
        writetag("KONTO", {Element::Int(konto.id), Element::String(konto.text)}, os);
        if (konto.sru > 0) {
            writetag("SRU", {Element::Int(konto.id), Element::Int(konto.sru)}, os);
        }
    }
    for (const auto& [konto, saldo] : siedata.balans_resultat[0].ib) {
        writetag("IB", {Element::Int(0), Element::Int(konto), Element::Valuta(saldo)}, os);
    }
    for (const auto& [konto, saldo] : siedata.balans_resultat[0].ub) {
        writetag("UB", {Element::Int(0), Element::Int(konto), Element::Valuta(saldo)}, os);
    }
    for (const auto& [konto, saldo] : siedata.balans_resultat[0].resultat) {
        writetag("RES", {Element::Int(0), Element::Int(konto), Element::Valuta(saldo)}, os);
    }
    for (const auto& verifikat : siedata.verifikat) {
        writetag("VER", {Element::String("V"), Element::String(std::to_string(verifikat.id)), Element::Int(verifikat.transaktionsdatum), Element::String(verifikat.text), Element::Int(verifikat.bokforingsdatum)}, os);
        os << "{" << std::endl;
        for (const auto& rad : verifikat.rader) {
            writerad(rad, os);
        }
        os << "}" << std::endl;
    }
    return true;
}
