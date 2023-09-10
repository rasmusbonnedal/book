#include "parsenumber.h"

namespace {

bool isdigit(char c) {
    return c >= '0' && c <= '9';
}

int digit_to_number(char c) {
    return c - '0';
}

}  // namespace

bool parse_number(const std::string_view& s, int64_t& number) {
    if (s.size() == 0) {
        return false;
    }
    bool sign = false;
    size_t cur = 0;
    number = 0;
    if (s[0] == '-') {
        if (s.size() == 1) {
            return false;
        }
        sign = true;
        cur++;
    }
    for (; cur < s.size(); ++cur) {
        char c = s[cur];
        if (!isdigit(c)) {
            return false;
        }
        number = number * 10 + digit_to_number(c);
    }
    if (sign) {
        number = -number;
    }
    return number;
}

bool parse_saldo(const std::string_view& s, int64_t& saldo) {
    enum ParseSaldoState { SIGN, WHOLE, DECIMAL } state = SIGN;
    saldo = 0;
    bool sign = false;
    int decimal_digits = 0;

    for (char c : s) {
        switch (state) {
            case SIGN:
                if (isdigit(c)) {
                    saldo = digit_to_number(c);
                    state = WHOLE;
                } else if (c == '-') {
                    sign = true;
                    state = WHOLE;
                } else {
                    return false;
                }
                break;
            case WHOLE:
                if (isdigit(c)) {
                    saldo = saldo * 10 + digit_to_number(c);
                } else if (c == '.') {
                    state = DECIMAL;
                } else {
                    return false;
                }
                break;
            case DECIMAL:
                if (decimal_digits < 2 && isdigit(c)) {
                    saldo = saldo * 10 + digit_to_number(c);
                    decimal_digits++;
                } else {
                    return false;
                }
                break;
        }
    }
    if (state == DECIMAL && decimal_digits == 0) {
        return false;
    }
    if (decimal_digits == 1) {
        saldo = saldo * 10;
    } else if (decimal_digits == 0) {
        saldo = saldo * 100;
    }
    if (sign) {
        if (saldo == 0) {
            return false;
        }
        saldo = -saldo;
    }
    return true;
}
