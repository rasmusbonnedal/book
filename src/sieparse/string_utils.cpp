#include "string_utils.h"

#include "cp437.h"

std::string_view trim(const std::string_view& s) {
    size_t start = s.find_first_not_of(" \t");
    if (start == std::string::npos) {
        return std::string_view();
    }
    size_t end = s.find_last_not_of("\n\r\t ");
    return s.substr(start, end - start + 1);
}

std::vector<std::string_view> split(const std::string_view& s) {
    std::vector<std::string_view> result;
    size_t start, next = 0;
    while (true) {
        start = s.find_first_not_of(" \t\n\r", next);
        if (start == std::string::npos) {
            return result;
        }
        next = s.find_first_of(" \t\n\r", start);
        result.push_back(s.substr(start, next - start));
    }
}

std::string convert_cp437_to_utf8(const std::string_view& s) {
    std::string output;
    output.reserve(s.size());
    for (unsigned char c : s) {
        if (c < 0x80) {
            output.push_back(c);
        } else {
            const unsigned char* utf8_code = cp437_to_utf8[c - 0x80];
            while (unsigned char x = *utf8_code++) {
                output.push_back(x);
            }
        }
    }
    return output;
}
