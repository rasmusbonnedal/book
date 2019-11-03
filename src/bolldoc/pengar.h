#pragma once

#include <iostream>
#include <string>

class Pengar {
public:
    Pengar();

    Pengar(const int64_t v);

    int64_t get() const;

    Pengar& operator+=(const Pengar& rhs);

private:
    int64_t _pengar;
};

Pengar parsePengar(const std::string& s);

std::string toString(const Pengar& p);

std::ostream& operator<<(std::ostream& stream, const Pengar& p);

bool operator==(const Pengar& lhs, const Pengar& rhs);
