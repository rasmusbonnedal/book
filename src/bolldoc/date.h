#pragma once

#include <iostream>
#include <optional>
#include <string>

class Date {
public:
    Date();

    Date(int year, int month, int day);

    int getYear() const;

    int getMonth() const;

    int getDay() const;

private:
    bool checkDate() const;

    int _year;
    int _month;
    int _day;
};

class DateRange {
public:
    DateRange(const Date& start, const Date& end);

    const Date& getStart() const;

    const Date& getEnd() const;

private:
    Date _start;
    Date _end;
};

Date parseDate(const std::string& s);

std::optional<Date> parseDateNothrow(const std::string& s);

bool operator<(const Date& lhs, const Date& rhs);

bool operator>=(const Date& lhs, const Date& rhs);

bool operator==(const Date& lhs, const Date& rhs);

bool operator!=(const Date& lhs, const Date& rhs);

std::ostream& operator<<(std::ostream& stream, const Date& d);

std::string to_string(const Date& d);

Date lastDayOfMonth(const Date& d);

Date now();

std::ostream& operator<<(std::ostream& stream, const DateRange& d);

std::string to_string(const DateRange& d);

DateRange fullYear(int year);
