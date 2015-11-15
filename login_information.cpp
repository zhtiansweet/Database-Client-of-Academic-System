//
// Created by Di Tian on 11/15/15.
//

#include "login_information.h"

// LoginInfo constructor
LoginInfo::LoginInfo(string new_id) {
    id = new_id;

    time_t theTime = time(nullptr);
    struct tm* aTime = localtime(&theTime);

    day = aTime->tm_mday;
    month = aTime->tm_mon + 1;
    year = aTime->tm_year + 1900;
    day_of_week = aTime->tm_wday;

    if (day_of_week == 1) {
        name_of_weekday = "Monday";
    } else if (day_of_week == 2) {
        name_of_weekday = "Tuesday";
    } else if (day_of_week == 3) {
        name_of_weekday = "Wednesday";
    } else if (day_of_week == 4) {
        name_of_weekday = "Thursday";
    } else if (day_of_week == 5) {
        name_of_weekday = "Friday";
    } else if (day_of_week == 6) {
        name_of_weekday = "Saturday";
    } else if (day_of_week == 0) {
        name_of_weekday = "Sunday";
    } else {
        name_of_weekday = "Invalid";
    }


    if (month==10 || month==11 || (month==9 && day>=15)
        || (month==12 && day<=12)) {  // Q1, fall quarter, Sep.15 -- Dec. 12
        quarter = "Q1";
    } else if (month==2 || (month==1 && day>=5) || (month==3 && day<=20)) {  // Q2, winter quarter, Jan. 5 -- Mar. 20
        quarter = "Q2";
    } else if (month==4 || month==5 || (month==3 && day>=30)
               || (month==6 && day<=19)) {  // Q3, spring quarter, Mar. 30 -- Jun. 19
        quarter = "Q3";
    } else if (month==7 || (month==6 && day>=22) || (month==8 && day<=15)) {  // Q4, summer quarter, Jun. 22 -- Aug. 15
        quarter = "Q4";
    } else {
        quarter = "vacation";
    }

    enrolled = new unordered_set<string>();
}


// LoginInfo destructor
LoginInfo::~LoginInfo() {
    delete enrolled;
}