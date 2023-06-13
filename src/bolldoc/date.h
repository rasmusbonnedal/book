#pragma once

#include <iostream>
#include <optional>
#include <string>

enum DateType {
    DATETYPE_FULL = 0,
    DATETYPE_Q1,
    DATETYPE_Q2,
    DATETYPE_Q3,
    DATETYPE_Q4,
    DATETYPE_JANUARY,
    DATETYPE_FEBRUARY,
    DATETYPE_MARCH,
    DATETYPE_APRIL,
    DATETYPE_MAY,
    DATETYPE_JUNE,
    DATETYPE_JULY,
    DATETYPE_AUGUST,
    DATETYPE_SEPTEMBER,
    DATETYPE_OCTOBER,
    DATETYPE_NOVEMBER,
    DATETYPE_DECEMBER,
    DATETYPE_COUNT
};

class Date {
public:
    Date();

    Date(int year, int month, int day);

    int getYear() const;

    int getMonth() const;

    int getDay() const;

    Date addDays(int days) const;

    static bool checkDate(int y, int m, int d);

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

void to_string(const Date& d, char* buf);

Date lastDayOfMonth(const Date& d);

Date now();

std::ostream& operator<<(std::ostream& stream, const DateRange& d);

std::string to_string(const DateRange& d);

DateRange fullYear(int year);

std::string dateTypeToString(const DateType t);

DateRange dateTypeToRange(const DateType t, int year);
