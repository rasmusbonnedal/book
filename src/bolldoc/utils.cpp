#include "utils.h"

#include <iostream>
#include <regex>

namespace {
    int64_t parseInt(const std::string& s) {
        if (s.length() == 0) return 0;
        return std::stoll(s);
    }
}

int64_t Utils::parsePengar(const std::string& s) {
    std::regex re("([-]?\\d+)([.]?)(\\d{0,2})");
    std::smatch m;
    if (!std::regex_match(s, m, re) || m.size() != 4) {
        throw std::runtime_error("Could not parse " + s + " as number");
    }

    int64_t whole = parseInt(m[1]);
    int64_t sign = whole < 0 ? -1 : 1;
    int64_t dec = parseInt(m[3]);
    if (m[3].str().size() == 1) dec *= 10;

    return whole * 100 + sign * dec;
}
