#include "pengar.h"

#include <iomanip>
#include <regex>
#include <sstream>

namespace {
int64_t parseInt(const std::string& s) {
    if (s.length() == 0)
        return 0;
    return std::stoll(s);
}
} // namespace

Pengar::Pengar() : _pengar(0) {}

Pengar::Pengar(const int64_t v) : _pengar(v) {}

Pengar& Pengar::operator+=(const Pengar& rhs) {
    _pengar += rhs.get();
    return *this;
}

int64_t Pengar::get() const { return _pengar; }

namespace {
    const std::regex pengarRegex("^([-]?)(\\d+)[.]?(\\d{0,2}) ?(?:kr)?$");
}

Pengar parsePengar(const std::string& s) {
    std::smatch m;
    if (!std::regex_match(s, m, pengarRegex) || m.size() != 4) {
        throw std::runtime_error("Could not parse " + s + " as number");
    }

    bool sign = m[1] == "-";
    int64_t whole = parseInt(m[2]);
    int64_t dec = parseInt(m[3]);
    if (m[3].str().size() == 1)
        dec *= 10;

    int64_t value = whole * 100 + dec;
    return sign ? -value : value;
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
