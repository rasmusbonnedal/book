#pragma once

#include <iostream>
#include <string>

class Pengar {
public:
    Pengar();

    Pengar(const int64_t v);

    int64_t get() const;

    Pengar& operator+=(const Pengar& rhs);

    Pengar operator-() const;

    Pengar operator+(const Pengar& rhs) const;

    Pengar operator-(const Pengar& rhs) const;

private:
    int64_t _pengar;
};

bool parsePengarNothrow(const std::string& s, Pengar& p);

Pengar parsePengar(const std::string& s);

std::string toString2(const Pengar& p);

void to_string(const Pengar& p, char* buf);

std::string to_string(const Pengar& p);

std::string toXmlString(const Pengar& p);

std::string toHtmlString(const Pengar& p);

std::ostream& operator<<(std::ostream& stream, const Pengar& p);

bool operator==(const Pengar& lhs, const Pengar& rhs);

bool operator!=(const Pengar& lhs, const Pengar& rhs);
