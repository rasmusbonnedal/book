#include <iostream>
#include <string>

class Date {
public:
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

Date parseDate(const std::string& s);

bool operator<(const Date& lhs, const Date& rhs);

bool operator==(const Date& lhs, const Date& rhs);

bool operator!=(const Date& lhs, const Date& rhs);

std::ostream& operator<<(std::ostream& stream, const Date& d);