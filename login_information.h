//
// Created by Di Tian on 11/15/15.
//

#ifndef DATABASE_LOGIN_INFORMATION_H
#define DATABASE_LOGIN_INFORMATION_H

#include <string>
#include <unordered_set>

using namespace std;

class LoginInfo {
private:
    string id;
    int year;
    int month;
    int day;
    int day_of_week;  // 0 -- Sunday,..., 6 -- Saturday
    string name_of_weekday;
    string quarter;
    unordered_set<string>* enrolled;

public:
    LoginInfo (string id);  // the constructor: declaration
    ~LoginInfo();  // the destructor: declaration

    string GetId() {return id;}
    int GetYear() {return year;}
    int GetMonth() {return month;}
    int GetDay() {return day;}
    int GetDayOfWeek() {return day_of_week;}
    string GetNameOfWeekDay() {return name_of_weekday;}
    string GetQuarter() {return quarter;}

};

#endif //DATABASE_LOGIN_INFORMATION_H
