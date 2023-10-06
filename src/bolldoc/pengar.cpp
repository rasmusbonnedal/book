#include "pengar.h"

#include "utils.h"

#include <iomanip>
#include <regex>
#include <sstream>

namespace {
int64_t parseInt(const std::string& s) {
    if (s.length() == 0)
        return 0;
    return std::stoll(s);
}

void toStream(std::ostream& stream, const Pengar& p, bool alwaysDecimal, bool comma) {
    int64_t value = p.get();
    int64_t kronor = value / 100;
    char buf[20];
    snprintf(buf, 20, "%lld", std::llabs(kronor));
    size_t len = strlen(buf);

    if (value < 0) {
        stream << '-';
    }
    for (size_t i = 0; i < len; ++i) {
        if (i > 0 && (len - i) % 3 == 0) {
            stream << ' ';
        }
        stream << buf[i];
    }

    int oren = std::abs(value) % 100;

    if (alwaysDecimal || oren != 0) {
        stream << (comma ? "," : ".") << std::setfill('0') << std::setw(2) << oren;
    }
}

} // namespace

Pengar::Pengar() : _pengar(0) {}

Pengar::Pengar(const int64_t v) : _pengar(v) {}

Pengar& Pengar::operator+=(const Pengar& rhs) {
    _pengar += rhs.get();
    return *this;
}

Pengar Pengar::operator-() const {
    return Pengar(-_pengar);
}

Pengar Pengar::operator+(const Pengar& rhs) const {
    return Pengar(_pengar + rhs._pengar);
}

Pengar Pengar::operator-(const Pengar& rhs) const {
    return Pengar(_pengar - rhs._pengar);
}

int64_t Pengar::get() const { return _pengar; }

namespace {
    const std::regex pengarRegex("^([-]?)(\\d+)[.,]?(\\d{0,2}) ?(?:kr)?$");
}

bool parsePengarNothrow(const std::string& s, Pengar& p) {
    std::string s2 = Utils::removeSpaces(s);

    std::smatch m;
    if (!std::regex_match(s2, m, pengarRegex) || m.size() != 4) {
        return false;
    }

    bool sign = m[1] == "-";
    int64_t whole = parseInt(m[2]);
    int64_t dec = parseInt(m[3]);
    if (m[3].str().size() == 1) dec *= 10;

    int64_t value = whole * 100 + dec;
    p = sign ? -value : value;
    return true;
}

Pengar parsePengar(const std::string& s) {
    Pengar p;
    if (!parsePengarNothrow(s, p)) {
        throw std::runtime_error("Could not parse " + s + " as number");
    }
    return p;
}

std::string toString2(const Pengar& p) {
    std::stringstream ss;
    ss << p;
    return ss.str();
}

std::string to_string(const Pengar& p) {
    char buf[64];
    to_string(p, buf);
    return std::string(buf);
}

void to_string(const Pengar& p, char* buf) {
    int64_t value = p.get();
    int64_t kronor = llabs(value / 100);
    int64_t r = 1000000000;

    if (value < 0) {
        *buf++ = '-';
        value = -value;
    }
    bool first = true;
    while (r > 0) {
        int64_t x = kronor / r;
        if (!first || x > 0) {
            kronor -= x * r;
            int y = snprintf(buf, 5, first ? "%d" : " %03d", (int)x);
            buf += y;
            first = false;
        }
        r /= 1000;
    }
    if (first) {
        *buf++ = '0';
    }
    int oren = llabs(value) % 100;
    if (oren != 0) {
        *buf++ = ',';
        *buf++ = '0' + (char)(oren / 10);
        *buf++ = '0' + (oren % 10);
    }
    *buf++ = ' ';
    *buf++ = 'k';
    *buf++ = 'r';
    *buf++ = '\0';
}

std::string toXmlString(const Pengar& p) {
    std::stringstream ss;
    int64_t value = p.get();
    int64_t kronor = value / 100;

    if (value < 0) {
        ss << '-';
    }
    ss << std::llabs(kronor);

    int oren = std::llabs(value) % 100;
    if (oren > 0) {
        ss << '.';
        if (oren < 10) {
            ss << '0' << oren;
        } else if ((oren % 10) == 0) {
            ss << (oren / 10);
        } else {
            ss << oren;
        }
    }
    return ss.str();
}

std::string toHtmlString(const Pengar& p) {
    std::stringstream ss;
    toStream(ss, p, true, true);
    return ss.str();
}

std::ostream& operator<<(std::ostream& stream, const Pengar& p) {
    toStream(stream, p, false, true);
    return stream;
}

bool operator==(const Pengar& lhs, const Pengar& rhs) {
    return lhs.get() == rhs.get();
}

bool operator!=(const Pengar& lhs, const Pengar& rhs) {
    return lhs.get() != rhs.get();
}
