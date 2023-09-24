#pragma once

#include <string_view>
#include <vector>

std::string_view trim(const std::string_view& s);
std::vector<std::string_view> split(const std::string_view& s);
std::string convert_cp437_to_utf8(const std::string_view& s);
std::string convert_utf8_to_cp437(const std::string_view& s);
