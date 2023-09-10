#pragma once

#include <string_view>

bool parse_number(const std::string_view& s, int64_t& number);

bool parse_saldo(const std::string_view& s, int64_t& saldo);
