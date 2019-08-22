#pragma once

#include <cstdint>
#include <string>

class Utils {
public:
    static uint32_t calcChecksum(const std::string& xmlText);
};
