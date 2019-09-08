#include "pengar.h"

#include <iomanip>
#include <regex>
#include <sstream>

namespace {
    int64_t parseInt(const std::string& s) {
        if (s.length() == 0) return 0;
        return std::stoll(s);
    }
}

Pengar::Pengar() : _pengar(0) {}

Pengar::Pengar(const int64_t v) : _pengar(v) {}

Pengar& Pengar::operator+=(const Pengar& rhs) {
    _pengar += rhs.get();
    return *this;
}

int64_t Pengar::get() const {
    return _pengar;
}

Pengar parsePengar(const std::string& s) {
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

std::string toString(const Pengar& p) {
    std::stringstream ss;
    ss << p;
    return ss.str();
}

std::ostream& operator<<(std::ostream& stream, const Pengar& p) {
    int64_t value = p.get();
    stream << (value / 100);
    int oren = std::abs(value) % 100;

    if (oren != 0) {
        stream << "." << std::setfill('0') << std::setw(2) << oren;
    }
    return stream;
}

bool operator==(const Pengar& lhs, const Pengar& rhs) {
    return lhs.get() == rhs.get();
}
