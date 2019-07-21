#include "utils.h"

#include <codecvt>
#include <iostream>
#include <locale>
#include <numeric>
#include <vector>

namespace {
    std::u16string toUTF16(const std::string& s) {
        std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> u16conv;
        return u16conv.from_bytes(s);
    }

    size_t findThrows(const std::string& s, const std::string& what, size_t start = 0) {
        size_t pos = s.find(what, start);
        if (pos == std::string::npos) throw std::runtime_error("Could not calculate checksum");
        return pos;
    }
}

uint32_t Utils::calcChecksum(const std::string& xmlText) {
    size_t start = findThrows(xmlText, ">", findThrows(xmlText, "<bollbok")) + 1;
    size_t length = findThrows(xmlText, "</bollbok>", start) - start;
    const std::u16string data = toUTF16(xmlText.substr(start, length));

    std::vector<int> sums((data.size() + 7) / 8, 0);
    for (size_t i = 0; i < data.size(); ++i) {
        sums[i / 8] += data[i];
    }
    return std::accumulate(sums.begin(), sums.end(), 1, 
        [](uint32_t l, uint32_t r) {
            return (l + r) * 0x6f4f53 + 0xb7f;
        });
}
