#include "sieparse.h"

#include <iostream>
#include <string_view>

#include "parsenumber.h"
#include "string_utils.h"

namespace {

bool parse_field(const std::string_view& line, std::string& tag, std::vector<std::string>& items) {
    enum ParseFieldState { ITEM, SPACE, BRACE, STRING, STRING_BACKSLASH };

    size_t tag_end = line.find_first_of(" \t");
    tag = line.substr(0, tag_end);

    size_t current = line.find_first_not_of(" \t", tag_end);
    ParseFieldState state = SPACE;
    std::string current_item;
    while (current < line.size()) {
        char c = line[current++];
        switch (state) {
            case ITEM: {
                if (c == ' ' || c == '\t') {
                    items.push_back(convert_cp437_to_utf8(current_item));
                    current_item.clear();
                    state = SPACE;
                } else {
                    current_item += c;
                }
                break;
            }
            case SPACE: {
                if (c == '{') {
                    current_item += c;
                    state = BRACE;
                } else if (c == '"') {
                    state = STRING;
                } else if (c != ' ' && c != '\t') {
                    current_item += c;
                    state = ITEM;
                }
                break;
            }
            case BRACE: {
                if (c == '}') {
                    current_item += c;
                    items.push_back(convert_cp437_to_utf8(current_item));
                    current_item.clear();
                    state = SPACE;
                } else {
                    current_item += c;
                }
                break;
            }
            case STRING: {
                if (c == '\\') {
                    state = STRING_BACKSLASH;
                } else if (c == '"') {
                    items.push_back(convert_cp437_to_utf8(current_item));
                    current_item.clear();
                    state = SPACE;
                } else {
                    current_item += c;
                }
                break;
            }
            case STRING_BACKSLASH: {
                if (c == '"') {
                    current_item += c;
                    state = STRING;
                } else {
                    current_item += '\\';
                    current_item += c;
                    state = STRING;
                }
            }
        }
    }
    if (state != ITEM && state != SPACE) {
        return false;
    }
    if (state == ITEM) {
        items.push_back(convert_cp437_to_utf8(current_item));
    }
    return true;
}

enum Token { OPEN_BRACE, CLOSE_BRACE, FIELD, EMPTY, INVALID };

Token tokenize(const std::string_view& strim) {
    if (strim.size() == 0) {
        return EMPTY;
    }
    if (strim.size() == 1) {
        if (strim[0] == '{') {
            return OPEN_BRACE;
        } else if (strim[0] == '}') {
            return CLOSE_BRACE;
        } else {
            return INVALID;
        }
    }
    if (strim[0] == '#') {
        return FIELD;
    } else {
        return INVALID;
    }
}

bool process_trans(const std::string_view& tag, const std::vector<std::string>& items, SIEVerifikat& verifikat) {
    // items[0]: account no
    // items[1]: object list
    // items[2]: amount
    // items[3]: transdate
    if (items.size() < 3) {
        return false;
    }
    SIETransaktion transaktion;
    if (tag == "#TRANS") {
        transaktion.typ = SIETransaktion::NORMAL;
    } else if (tag == "#RTRANS") {
        transaktion.typ = SIETransaktion::RTRANS;
    } else if (tag == "#BTRANS") {
        transaktion.typ = SIETransaktion::BTRANS;
    } else {
        return false;
    }
    if (!parse_number(items[0], transaktion.konto)) {
        return false;
    }
    if (!parse_saldo(items[2], transaktion.saldo)) {
        return false;
    }
    verifikat.rader.push_back(std::move(transaktion));
    return true;
}

bool process_verifikat(const std::vector<std::string>& items, SIEVerifikat& verifikat) {
    verifikat = SIEVerifikat();
    if (items.size() < 3) {
        return false;
    }

    // items[0]: series (string)
    // items[1]: verno
    // items[2]: verdate
    // items[3]: vertext
    // items[4]: regdate
    // items[5]: sign

    if (!parse_number(items[1], verifikat.id)) {
        return false;
    }
    if (!parse_number(items[2], verifikat.transaktionsdatum)) {
        return false;
    }
    if (items.size() > 3) {
        verifikat.text = items[3];
    }
    if (items.size() > 4) {
        if (!parse_number(items[4], verifikat.bokforingsdatum)) {
            return false;
        }
    }
    return true;
}

bool process_konto(const std::vector<std::string>& items, SIEKonto& konto) {
    if (items.size() < 2) {
        return false;
    }
    // items[0]: id
    // items[1]: text
    int64_t id;
    if (!parse_number(items[0], id)) {
        return false;
    }
    konto.id = (int)id;
    konto.text = items[1];
    return true;
}

bool process_sru(const std::vector<std::string>& items, SIEKonto& konto) {
    if (items.size() < 2) {
        return false;
    }
    // items[0]: id
    // items[1]: sru
    int64_t id;
    if (!parse_number(items[0], id)) {
        return false;
    }
    int64_t sru;
    if (!parse_number(items[1], sru)) {
        return false;
    }
    konto.id = (int)id;
    konto.sru = (int)sru;
    return true;
}

bool process_rar(const std::vector<std::string>& items, SIEData& siedata) {
    if (items.size() < 3) {
        return false;
    }
    // items[0]: year_id
    // items[1]: start date
    // items[2]: end date

    int64_t id;
    if (!parse_number(items[0], id)) {
        return false;
    }
    int64_t start;
    if (!parse_number(items[1], start)) {
        return false;
    }
    int64_t end;
    if (!parse_number(items[2], end)) {
        return false;
    }
    if (id == 0) {
        siedata.rakenskapsar_start = (int)start;
        siedata.rakenskapsar_slut = (int)end;
    }
    return true;
}

bool process_ib(const std::vector<std::string>& items, SIEData& siedata) {
    if (items.size() < 3) {
        return false;
    }
    // items[0]: year_id
    // items[1]: konto
    // items[2]: amount
    int64_t id;
    if (!parse_number(items[0], id)) {
        return false;
    }
    int64_t konto;
    if (!parse_number(items[1], konto)) {
        return false;
    }
    int64_t saldo;
    if (!parse_saldo(items[2], saldo)) {
        return false;
    }

    siedata.balans_resultat[(int)id].ib[(int)konto] = saldo;
    return true;
}

bool process_ub(const std::vector<std::string>& items, SIEData& siedata) {
    if (items.size() < 3) {
        return false;
    }
    // items[0]: year_id
    // items[1]: konto
    // items[2]: amount
    int64_t id;
    if (!parse_number(items[0], id)) {
        return false;
    }
    int64_t konto;
    if (!parse_number(items[1], konto)) {
        return false;
    }
    int64_t saldo;
    if (!parse_saldo(items[2], saldo)) {
        return false;
    }

    siedata.balans_resultat[(int)id].ub[(int)konto] = saldo;
    return true;
}

bool process_res(const std::vector<std::string>& items, SIEData& siedata) {
    if (items.size() < 3) {
        return false;
    }
    // items[0]: year_id
    // items[1]: konto
    // items[2]: amount
    int64_t id;
    if (!parse_number(items[0], id)) {
        return false;
    }
    int64_t konto;
    if (!parse_number(items[1], konto)) {
        return false;
    }
    int64_t saldo;
    if (!parse_saldo(items[2], saldo)) {
        return false;
    }

    siedata.balans_resultat[(int)id].resultat[(int)konto] = saldo;
    return true;
}

bool parse_error(const std::string& line, int line_number, const std::string& message) {
    std::cerr << "Parse error line #" << line_number << ", " << message << std::endl;
    std::cerr << "\"" << line << "\"" << std::endl;
    return false;
}

bool verify_balances(const SIEData& siedata) {
    int64_t ib = 0;
    int64_t ub = 0;
    int64_t resultat = 0;
    auto it = siedata.balans_resultat.find(0);
    if (it != siedata.balans_resultat.end()) {
        const SIEBalansResultat& br = it->second;
        for (const auto& [k, v] : br.ib) {
            ib += v;
        }
        for (const auto& [k, v] : br.ub) {
            ub += v;
        }
        for (const auto& [k, v] : br.resultat) {
            resultat += v;
        }
    }
    if (ib != 0 || ub != -resultat) {
        std::cerr << "Balances doesn't match, IB = " << ib << ", UB = " << ub << ", Resultat = " << resultat << std::endl;
        return false;
    }
    return true;
}

}  // namespace

bool parse(SIEData& siedata, std::istream& is) {
    enum States { NORMAL, EXPECT_SUBENTRY, IN_SUBENTRY };
    std::string line;
    States parse_state = NORMAL;

    // Current verifikat, only valid when in { EXPECT_SUBENTRY, IN_SUBENTRY }
    SIEVerifikat verifikat;

    int line_number = 0;
    while (std::getline(is, line)) {
        line_number++;
        std::string_view strim = trim(line);
        Token token = tokenize(strim);
        if (token == EMPTY) {
            continue;
        }
        if (token == INVALID) {
            return parse_error(line, line_number, "invalid token");
        }
        if (parse_state == NORMAL) {
            switch (token) {
                case FIELD: {
                    std::string tag;
                    std::vector<std::string> items;
                    if (!parse_field(strim, tag, items)) {
                        return parse_error(line, line_number, "parsing field");
                    }
                    if (tag == "#VER") {
                        parse_state = EXPECT_SUBENTRY;
                        if (!process_verifikat(items, verifikat)) {
                            return parse_error(line, line_number, "processing #VER");
                        }
                    } else if (tag == "#KONTO") {
                        SIEKonto konto;
                        if (!process_konto(items, konto)) {
                            return parse_error(line, line_number, "processing #KONTO");
                        }
                        if (siedata.kontoplan.count(konto.id) > 0) {
                            siedata.kontoplan[konto.id].text = konto.text;
                        } else {
                            siedata.kontoplan[konto.id] = konto;
                        }
                    } else if (tag == "#SRU") {
                        SIEKonto konto;
                        if (!process_sru(items, konto)) {
                            return parse_error(line, line_number, "processing #SRU");
                        }
                        if (siedata.kontoplan.count(konto.id) > 0) {
                            siedata.kontoplan[konto.id].sru = konto.sru;
                        } else {
                            siedata.kontoplan[konto.id] = konto;
                        }
                    } else if (tag == "#RAR") {
                        if (!process_rar(items, siedata)) {
                            return parse_error(line, line_number, "processing #RAR");
                        }
                    } else if (tag == "#IB") {
                        if (!process_ib(items, siedata)) {
                            return parse_error(line, line_number, "processing #IB");
                        }
                    } else if (tag == "#UB") {
                        if (!process_ub(items, siedata)) {
                            return parse_error(line, line_number, "processing #UB");
                        }
                    } else if (tag == "#RES") {
                        if (!process_res(items, siedata)) {
                            return parse_error(line, line_number, "processing #RES");
                        }
                    } else {
                        // std::cout << "TAG: " << tag << std::endl;
                        siedata.fields[tag] = items;
                    }

                    break;
                }
                case OPEN_BRACE:
                case CLOSE_BRACE:
                    std::cerr << "Parse error line #" << line_number << ", did not expect brace" << std::endl;
                    std::cerr << "\"" << line << "\"" << std::endl;
                    return false;
            }
        } else if (parse_state == EXPECT_SUBENTRY) {
            switch (token) {
                case OPEN_BRACE:
                    parse_state = IN_SUBENTRY;
                    break;
                case CLOSE_BRACE:
                case FIELD:
                    std::cerr << "Parse error line #" << line_number << ", expected {" << std::endl;
                    std::cerr << "\"" << line << "\"" << std::endl;
                    return false;
            }
        } else if (parse_state == IN_SUBENTRY) {
            switch (token) {
                case CLOSE_BRACE:
                    parse_state = NORMAL;
                    siedata.verifikat.push_back(std::move(verifikat));
                    verifikat = SIEVerifikat();
                    break;
                case FIELD: {
                    std::string tag;
                    std::vector<std::string> items;
                    if (!parse_field(strim, tag, items)) {
                        std::cerr << "Parse error line #" << line_number << ", error parsing field" << std::endl;
                        std::cerr << "\"" << line << "\"" << std::endl;
                        return false;
                    }
                    if (tag == "#TRANS" || tag == "#RTRANS" || tag == "#BTRANS") {
                        if (!process_trans(tag, items, verifikat)) {
                            std::cerr << "Parse error line #" << line_number << ", error parsing tag" << std::endl;
                            std::cerr << "\"" << line << "\"" << std::endl;
                            return false;
                        }
                    } else {
                        std::cerr << "Parse error line #" << line_number << ", tag " << tag << " not allowed" << std::endl;
                        std::cerr << "\"" << line << "\"" << std::endl;
                    }
                    break;
                }
                case OPEN_BRACE:
                    std::cerr << "Parse error line #" << line_number << ", expected #TRANS or }" << std::endl;
                    std::cerr << "\"" << line << "\"" << std::endl;
                    return false;
            }
        }
    }
    if (siedata.fields["#SIETYP"][0] != "4") {
        std::cerr << "Only #SIETYP 4 supported (this is type " << siedata.fields["#SIETYP"][0] << ")" << std::endl;
        return false;
    }
    auto& fnamn = siedata.fields["#FNAMN"];
    if (fnamn.size() < 1) {
        std::cerr << "#FNAMN not specified" << std::endl;
        return false;
    }
    siedata.foretags_namn = fnamn[0];

    auto& orgnr = siedata.fields["#ORGNR"];
    if (orgnr.size() < 1) {
        std::cerr << "#ORGNR not specified" << std::endl;
        return false;
    }
    if (orgnr[0].size() != 10) {
        std::cerr << "#ORGNR not 10 digits" << std::endl;
        return false;
    }
    siedata.org_nummer = orgnr[0].substr(0, 6) + "-" + orgnr[0].substr(6, 4);

    return verify_balances(siedata);
}
