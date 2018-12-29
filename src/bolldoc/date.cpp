#include "date.h"

#include <iomanip>
#include <regex>
#include <string>
#include <sstream>

namespace {
    bool isLeapYear(int year) {
        return ((year % 4 == 0) && (year % 100 != 0)) || (year % 400 == 0);
    }    
}

Date::Date(int year, int month, int day) 
: _year(year)
, _month(month)
, _day(day) {
    if (!checkDate()) {
        std::stringstream ss;
        ss << "Invalid date " << *this;
        throw std::runtime_error(ss.str());
    }
}

int Date::getYear() const {
    return _year;
}

int Date::getMonth() const {
    return _month;
}

int Date::getDay() const {
    return _day;
}

bool Date::checkDate() const {
    if (_year < 0 || _month < 1 || _month > 12 || _day < 1) return false;
    switch(_month) {
    case 1:
    case 3:
    case 5:
    case 7:
    case 8:
    case 10:
    case 12:
        if (_day > 31) return false;
        break;
    case 4:
    case 6:
    case 9:
    case 11:
        if (_day > 30) return false;
        break;
    case 2:
        if (_day > (isLeapYear(_year) ? 29 : 28)) return false;
        break;
    }
    return true;
}

Date parseDate(const std::string& s) {
    std::regex re("(\\d{4})-(\\d\\d)-(\\d\\d)");
    std::smatch m;
    if (!std::regex_match(s, m, re) || m.size() != 4) {
        throw std::runtime_error("Could not parse " + s + " as a date");
    }

    return Date(std::stoi(m[1]), std::stoi(m[2]), std::stoi(m[3]));
}

bool operator<(const Date& lhs, const Date& rhs) {
    if (lhs.getYear() == rhs.getYear()) {
        if (lhs.getMonth() == rhs.getMonth()) {
            return lhs.getDay() < rhs.getDay();
        } else {
            return lhs.getMonth() < rhs.getMonth();
        }
    } else {
        return lhs.getYear() < rhs.getYear();
    }
}

bool operator==(const Date& lhs, const Date& rhs) {
    return lhs.getYear() == rhs.getYear() &&
           lhs.getMonth() == rhs.getMonth() &&
           lhs.getDay() == rhs.getDay();
}

bool operator!=(const Date& lhs, const Date& rhs) {
    return !(lhs == rhs);
}

std::ostream& operator<<(std::ostream& stream, const Date& d) {
    stream << std::setfill('0') << std::setw(4) << d.getYear() << "-" 
           << std::setw(2) << d.getMonth() << "-" << std::setw(2) << d.getDay();
    return stream;
}

