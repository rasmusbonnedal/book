#include <iostream>
#include <string>

class Pengar {
public:
    Pengar();

    Pengar(const int64_t v);

    int64_t get() const;

private:
    int64_t _pengar;
};

Pengar parsePengar(const std::string& s);

std::string toString(const Pengar& p);

std::ostream& operator<<(std::ostream& stream, const Pengar& p);
