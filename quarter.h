//
// Created by Di Tian on 11/15/15.
//

#ifndef DATABASE_QUARTER_H
#define DATABASE_QUARTER_H

#include <string>
using namespace std;

class Quarter {
private:
    int school_year;
    int quarter_id;
    string quarter_name;

public:
    Quarter(int cur_year, int cur_month, int cur_day);  // constructor 0
    Quarter(int schoolyear, int quarterid);  // constructor 1

    int GetQuarter_SchoolYear() {return school_year;}
    string GetQuarter_Name() {return quarter_name;}
    Quarter* GetNextQuarter();
};


#endif //DATABASE_QUARTER_H
