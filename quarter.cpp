//
// Created by Di Tian on 11/15/15.
//

#include "quarter.h"


Quarter::Quarter(int cur_year, int cur_month, int cur_day) {
    if (cur_month==10 || cur_month==11 || (cur_month==9 && cur_day>=15)
        || (cur_month==12 && cur_day<=12)) {  // Q1, fall quarter, Sep.15 -- Dec. 12
        quarter_id = 1;
        quarter_name = "Q1";
    } else if (cur_month==2 || (cur_month==1 && cur_day>=5) || (cur_month==3 && cur_day<=20)) {
        // Q2, winter quarter, Jan. 5 -- Mar. 20
        quarter_id = 2;
        quarter_name = "Q2";
    } else if (cur_month==4 || cur_month==5 || (cur_month==3 && cur_day>=30)
               || (cur_month==6 && cur_day<=19)) {  // Q3, spring quarter, Mar. 30 -- Jun. 19
        quarter_id = 3;
        quarter_name = "Q3";
    } else if (cur_month==7 || (cur_month==6 && cur_day>=22) || (cur_month==8 && cur_day<=15)) {
        // Q4, summer quarter, Jun. 22 -- Aug. 15
        quarter_id = 4;
        quarter_name = "Q4";
    } else {
        quarter_id = 0;
        quarter_name = "Vacation";
    }

    if (quarter_id == 1 || quarter_id == 0) {  // Q1 belongs to current school year
        school_year = cur_year;
    } else {
        school_year = cur_year - 1;  // Q2, Q3, Q4 belong to last school year
    }
}


Quarter::Quarter(int schoolyear, int quarterid) {
    school_year = schoolyear;
    quarter_id = quarterid;
    string qname = "Q";
    qname.push_back('0' + quarterid);
    quarter_name = qname;
}


Quarter* Quarter::GetNextQuarter() {
    int nq_id = (quarter_id % 4) + 1;
    int nq_school_year = (quarter_id == 4) ? (school_year + 1) : (school_year);
    Quarter* ptr = new Quarter(nq_school_year, nq_id);
    return ptr;
}