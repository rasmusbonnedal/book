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

std::string convert_utf8_to_cp437(const std::string_view& s) {
    std::string output;
    for (int i = 0; i < s.size(); ++i) {
        unsigned char c = s[i];
        int len = 0;
        if ((c & 0x80) == 0) { // 1-byte
            output.push_back(c);
        } else if ((c & 0xf0) == 0xf0) { // 4-byte
            len = 4;
        } else if ((c & 0xe0) == 0xe0) { // 3-byte
            len = 3;            
        } else if ((c & 0xc0) == 0xc0) { // 2-byte
            len = 2;
        }
        if (len > 0) {
            if (i + len - 1 >= s.size()) {
                break;
            }
            for (int j = 0; j < 0x80; j++) {
                bool valid = true;
                for (int k = 0; k < len; ++k) {
                    unsigned char c0 = cp437_to_utf8[j][k];
                    unsigned char c1 = s[i + k];
                    if (c0 != c1) {
                        valid = false;
                        break;
                    }
                }
                if (valid) {
                    output.push_back((unsigned char)(j + 0x80));
                    break;
                }
            }
        }
    }
    return output;
}
