#include <cstdint>
#include <string>

class Utils {
public:
    static int64_t parsePengar(const std::string& s);
    static uint32_t calcChecksum(const std::string& xmlText);
};
