#include "date.h"

#include <chrono>
#include <time.h>
#include <iomanip>
#include <regex>
#include <sstream>
#include <string>

namespace {
bool isLeapYear(int year) {
    return ((year % 4 == 0) && (year % 100 != 0)) || (year % 400 == 0);
}

int lastDayOfMonth(int year, int month) {
    switch (month) {
    case 1:
    case 3:
    case 5:
    case 7:
    case 8:
    case 10:
    case 12:
        return 31;
    case 4:
    case 6:
    case 9:
    case 11:
        return 30;
    case 2:
        return isLeapYear(year) ? 29 : 28;
    default:
        std::stringstream ss;
        ss << "Invalid month " << month;
        throw std::runtime_error(ss.str());
    }
}
} // namespace

Date::Date() : _year(0), _month(0), _day(0) {}

Date::Date(int year, int month, int day)
    : _year(year), _month(month), _day(day) {
    if (!checkDate()) {
        std::stringstream ss;
        ss << "Invalid date " << *this;
        throw std::runtime_error(ss.str());
    }
}

int Date::getYear() const { return _year; }

int Date::getMonth() const { return _month; }

int Date::getDay() const { return _day; }

Date Date::addDays(int days) const {
    if (days <= 0) {
        throw std::runtime_error("Could not add negative or zero days");
    }
    Date d(*this);
    d._day += days;
    while (true) {
        int ldom = lastDayOfMonth(d._year, d._month);
        if (d._day <= ldom) {
            break;
        }
        d._day -= ldom;
        d._month++;
        if (d._month > 12) {
            d._month = 1;
            d._year++;
        }
    }
    return d;
}

bool Date::checkDate() const {
    return checkDate(_year, _month, _day);
}

bool Date::checkDate(int y, int m, int d) {
    if (y < 0 || m < 1 || m > 12 || d < 1) return false;
    return d <= lastDayOfMonth(y, m);
}

DateRange::DateRange(const Date& start, const Date& end)
    : _start(start), _end(end) {}

const Date& DateRange::getStart() const { return _start; }

const Date& DateRange::getEnd() const { return _end; }

bool DateRange::isInRange(const Date& d) const {
    return d >= _start && d <= _end;
}

namespace {
const std::regex dateRegex("(\\d{4})-(\\d\\d)-(\\d\\d)");
}

Date parseDate(const std::string& s) {
    auto date = parseDateNothrow(s);
    if (date) {
        return *date;
    }
    throw std::runtime_error("Could not parse " + s + " as a date");
}

std::optional<Date> parseDateNothrow(const std::string& s) {
    std::smatch m;
    if (std::regex_match(s, m, dateRegex) && m.size() == 4) {
        int year = std::stoi(m[1]);
        int month = std::stoi(m[2]);
        int day = std::stoi(m[3]);
        if (Date::checkDate(year, month, day)) {
            return Date(year, month, day);
        }
    }
    return std::nullopt;
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

bool operator>(const Date& lhs, const Date& rhs) {
    return rhs < lhs;
}

bool operator>=(const Date& lhs, const Date& rhs) {
    return !(lhs < rhs);
}

bool operator<=(const Date& lhs, const Date& rhs) {
    return !(rhs < lhs);
}

bool operator==(const Date& lhs, const Date& rhs) {
    return lhs.getYear() == rhs.getYear() && lhs.getMonth() == rhs.getMonth() &&
           lhs.getDay() == rhs.getDay();
}

bool operator!=(const Date& lhs, const Date& rhs) { return !(lhs == rhs); }

std::ostream& operator<<(std::ostream& stream, const Date& d) {
    stream << std::setfill('0') << std::setw(4) << d.getYear() << "-"
           << std::setw(2) << d.getMonth() << "-" << std::setw(2) << d.getDay();
    return stream;
}

std::string to_string(const Date& d) {
    std::stringstream ss;
    ss << d;
    return ss.str();
}

void to_string(const Date& d, char* buf) {
    snprintf(buf, 11, "%04d-%02d-%02d", d.getYear(), d.getMonth(), d.getDay());
}

Date lastDayOfMonth(const Date& d) {
    return Date(d.getYear(), d.getMonth(),
                lastDayOfMonth(d.getYear(), d.getMonth()));
}

Date now() {
    auto t = std::chrono::system_clock::now();
    std::time_t tt = std::chrono::system_clock::to_time_t(t);
    auto tm = std::tm{0};
#if defined(_WIN32)
    gmtime_s(&tm, &tt);
#else
    gmtime_r(&tt, &tm);
#endif
    return Date(tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
}

bool operator!=(const DateRange& lhs, const DateRange& rhs) {
    return lhs.getStart() != rhs.getStart() || lhs.getEnd() != rhs.getEnd();
}

std::ostream& operator<<(std::ostream& stream, const DateRange& d) {
    stream << d.getStart() << " - " << d.getEnd();
    return stream;
}

std::string to_string(const DateRange& d) {
    std::stringstream ss;
    ss << d;
    return ss.str();
}

DateRange fullYear(int year) { return {Date(year, 1, 1), Date(year, 12, 31)}; }

const char* dateTypeToString(const DateType t) {
    switch (t) {
    case DATETYPE_FULL:
        return "Hela året";
    case DATETYPE_Q1:
        return "Q1";
    case DATETYPE_Q2:
        return "Q2";
    case DATETYPE_Q3:
        return "Q3";
    case DATETYPE_Q4:
        return "Q4";
    case DATETYPE_JANUARY:
        return "Januari";
    case DATETYPE_FEBRUARY:
        return "Februari";
    case DATETYPE_MARCH:
        return "Mars";
    case DATETYPE_APRIL:
        return "April";
    case DATETYPE_MAY:
        return "Maj";
    case DATETYPE_JUNE:
        return "Juni";
    case DATETYPE_JULY:
        return "Juli";
    case DATETYPE_AUGUST:
        return "Augusti";
    case DATETYPE_SEPTEMBER:
        return "September";
    case DATETYPE_OCTOBER:
        return "Oktober";
    case DATETYPE_NOVEMBER:
        return "November";
    case DATETYPE_DECEMBER:
        return "December";
    case DATETYPE_COUNT:;
    }
    throw std::runtime_error("Invalid DateType " + std::to_string(t));
}

DateRange dateTypeToRange(const DateType t, int year) {
    switch (t) {
    case DATETYPE_FULL:
        return fullYear(year);
    case DATETYPE_Q1:
    case DATETYPE_Q2:
    case DATETYPE_Q3:
    case DATETYPE_Q4:
    {
        int month = ((int)t - (int)DATETYPE_Q1) * 3 + 1;
        return DateRange({year, month, 1}, {year, month + 2, lastDayOfMonth(year, month + 2)});
    }
    case DATETYPE_JANUARY:
    case DATETYPE_FEBRUARY:
    case DATETYPE_MARCH:
    case DATETYPE_APRIL:
    case DATETYPE_MAY:
    case DATETYPE_JUNE:
    case DATETYPE_JULY:
    case DATETYPE_AUGUST:
    case DATETYPE_SEPTEMBER:
    case DATETYPE_OCTOBER:
    case DATETYPE_NOVEMBER:
    case DATETYPE_DECEMBER:
    {
        int month = (int)t - (int)DATETYPE_JANUARY + 1;
        return DateRange({year, month, 1}, {year, month, lastDayOfMonth(year, month)});
    }
    case DATETYPE_COUNT:;
    }
    throw std::runtime_error("Invalid DateType " + std::to_string(t));
}
