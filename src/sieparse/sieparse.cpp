#include "sieparse.h"

#include <iostream>
#include <string_view>

#include "parsenumber.h"

namespace {

std::string_view trim(const std::string_view& s) {
    size_t start = s.find_first_not_of(" \t");
    if (start == std::string::npos) {
        return std::string_view();
    }
    size_t end = s.find_last_not_of("\n\r\t ");
    return s.substr(start, end - start + 1);
}

std::vector<std::string_view> split(const std::string_view& s) {
    std::vector<std::string_view> result;
    size_t start, next = 0;
    while (true) {
        start = s.find_first_not_of(" \t\n\r", next);
        if (start == std::string::npos) {
            return result;
        }
        next = s.find_first_of(" \t\n\r", start);
        result.push_back(s.substr(start, next - start));
    }
}

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
                    items.push_back(std::move(current_item));
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
                    items.push_back(std::move(current_item));
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
                    items.push_back(std::move(current_item));
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
        items.push_back(std::move(current_item));
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

}  // namespace


bool parse(SIEData& siedata, std::istream& is) {
    (void)siedata;
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
            std::cerr << "Parse error line #" << line_number << ", invalid token" << std::endl;
            std::cerr << "\"" << line << "\"" << std::endl;
            return false;
        }
        if (parse_state == NORMAL) {
            switch (token) {
                case FIELD: {
                    std::string tag;
                    std::vector<std::string> items;
                    if (!parse_field(strim, tag, items)) {
                        std::cerr << "Parse error line #" << line_number << ", error parsing field" << std::endl;
                        std::cerr << "\"" << line << "\"" << std::endl;
                        return false;
                    }
                    if (tag == "#VER") {
                        parse_state = EXPECT_SUBENTRY;
                        if (!process_verifikat(items, verifikat)) {
                            std::cerr << "Parse error line #" << line_number << ", error processing #VER" << std::endl;
                            std::cerr << "\"" << line << "\"" << std::endl;
                            return false;
                        }
                    } else if (tag == "#KONTO") {
                        SIEKonto konto;
                        if (!process_konto(items, konto)) {
                            std::cerr << "Parse error line #" << line_number << ", error processing #KONTO" << std::endl;
                            std::cerr << "\"" << line << "\"" << std::endl;
                            return false;
                        }
                        if (siedata.kontoplan.count(konto.id) > 0) {
                            siedata.kontoplan[konto.id].text = konto.text;
                        }
                        else {
                            siedata.kontoplan[konto.id] = konto;
                        }
                    } else {
                        std::cout << "TAG: " << tag << std::endl;
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
    return true;
}
