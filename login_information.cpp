//
// Created by Di Tian on 11/15/15.
//

#include "login_information.h"

// LoginInfo constructor
LoginInfo::LoginInfo(string new_id) {
    id = new_id;

    time_t theTime = time(nullptr);
    struct tm* aTime = localtime(&theTime); /* Points to a static internal std:tm on success, on NULL otherwise.
The structure may be shared between std::gmtime, std::localtime, and std::ctime, and may be overwritten on each
invocation. SO DO NOT DEALLOCATE IT MANUALLY! */

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

    current_quarter = new Quarter(year, month, day);
    next_quarter = current_quarter->GetNextQuarter();

    //enrolled = new unordered_set<string>();
}


// LoginInfo destructor
LoginInfo::~LoginInfo() {
    delete current_quarter;
    delete next_quarter;
    //delete enrolled;
}