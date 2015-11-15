//
// Created by Di Tian on 11/13/15.
//
#include <mysql.h>
#include <iostream>
#include <string>
#include <ctime>

#include "utility.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
using namespace std;

MYSQL* initialize() {
    MYSQL *conn = mysql_init(nullptr);
    if (conn == nullptr) {
        cout << "Insufficient memory!" << endl;
        exit(0);
    }
    return conn;
}

void connect(MYSQL* conn) {
    if (conn == nullptr) {
        cout << "Null pointer." << endl;
        exit(0);
    }
    if (mysql_real_connect(conn, "localhost", "root", "12345", "project3-nudb", 0, 0, 0) == nullptr) {
        cout << "Connection failed!" << endl;  // unable to connect
    } else {
        cout << "Connection succeeded!" << endl;
    }
}

void close(MYSQL* conn) {
    if (conn == nullptr) {
        cout << "Null pointer." << endl;
        exit(0);
    }
    mysql_close(conn);
}

void login(MYSQL* conn) {
    while (true) {
        string student_id;
        string password;
        cout << "Username: ";  // TODO: 1). int type check; 2). int length check.
        cin >> student_id;
        cout << "Password: ";
        cin >> password;

        MYSQL_RES *res_set;
        MYSQL_ROW row;

        if (mysql_query(conn, ("select * from student where id = " + student_id).c_str())) {
            error(conn);
        }

        res_set = mysql_store_result(conn);
        int num_rows = (int) mysql_num_rows(res_set);

        if (num_rows == 0) {
            cout << "Student doesn't exist!" << endl;
        } else {
            row = mysql_fetch_row(res_set);
            if (row[2] == password) {
                cout << endl << "You are logged in as " <<  student_id << "." << endl;
                student_menu(conn, student_id);
                break;
            } else {
                cout << "Wrong password:(" << endl;
            }
        }
        mysql_free_result(res_set);
    }
}

void student_menu(MYSQL* conn, const string& student_id) {
    time_t theTime = time(nullptr);
    struct tm* aTime = localtime(&theTime);
    int day = aTime->tm_mday;
    int month = aTime->tm_mon + 1;
    int year = aTime->tm_year + 1900;
    int day_of_week = aTime->tm_wday;
    string weekday_name = "";
    switch (day_of_week) {
        case 1:
            weekday_name = "Monday";
            break;
        case 2:
            weekday_name = "Tuesday";
            break;
        case 3:
            weekday_name = "Wednesday";
            break;
        case 4:
            weekday_name = "Thursday";
            break;
        case 5:
            weekday_name = "Friday";
            break;
        case 6:
            weekday_name = "Saturday";
            break;
        case 7:
            weekday_name = "Sunday";
            break;
        default:
            break;
    }
    cout << "Today is " << month << "-" << day << "-" << year << ", " << weekday_name << "." << endl << endl;

    string quarter_name = "";
    if (month==10 || month==11 || (month==9 && day>=15)
        || (month==12 && day<=12)) {  // Q1, fall quarter, Sep.15 -- Dec. 12
        quarter_name = "Q1";
    } else if (month==2 || (month==1 && day>=5) || (month==3 && day<=20)) {  // Q2, winter quarter, Jan. 5 -- Mar. 20
        quarter_name = "Q2";
    } else if (month==4 || month==5 || (month==3 && day>=30)
               || (month==6 && day<=19)) {  // Q3, spring quarter, Mar. 30 -- Jun. 19
        quarter_name = "Q3";
    } else if (month==7 || (month==6 && day>=22) || (month==8 && day<=15)) {  // Q4, summer quarter, Jun. 22 -- Aug. 15
        quarter_name = "Q4";
    } else {
        quarter_name = "vacation";
    }

    string stmt_str = "select unitofstudy.UoSCode, unitofstudy.UoSName"
                      " from student"
                      " join transcript on student.Id = transcript.StudId"
                      " join uosoffering on transcript.UoSCode = uosoffering.UoSCode and"
                                              " transcript.Semester = uosoffering.Semester and"
                                              " transcript.Year = uosoffering.Year"
                      " join unitofstudy on transcript.UoSCode = unitofstudy.UoSCode";
                      //" where student.Id = \"%s\" and uosoffering.Year = %d and uosoffering.Semester = \"%s\";";
    stmt_str += " where student.Id = " + student_id +
                " and uosoffering.Year = " + to_string(year) +
                " and uosoffering.Semester = \"" + quarter_name + "\";";

    if (mysql_query(conn, stmt_str.c_str())) {
        error(conn);
    }

    MYSQL_RES* res_set = mysql_store_result(conn);
    int num_rows = (int) mysql_num_rows(res_set);
    if (num_rows == 0) {
        cout << "Yeah! You are not enrolled in any course!" << endl;
    } else {
        cout << " -------------------------------------------" << endl;
        cout << "| You are enrolled in the following courses |" << endl;
        cout << " -------------------------------------------" << endl;
        for (int i=0; i<num_rows; ++i) {
            MYSQL_ROW row = mysql_fetch_row(res_set);
            cout << row[0] <<  "    " << row[1] << endl;
        }
        cout << endl;
        //cout << "--------------------------------------------" << endl;
    }
    mysql_free_result(res_set);

    cout << " -------------------------------------------" << endl;
    cout << "| You can proceed to the following options  |" << endl;
    cout << " -------------------------------------------" << endl;
    cout << "1. Transcript" << endl << "2. Enroll" << endl << "3. Withdrow" << endl
         << "4. Personal Details" << endl << "5. Logout" << endl;
    cout << endl;
    //cout << "--------------------------------------------" << endl;

    while (true) {
        cout << "Please select: ";
        int option = 0;
        cin >> option;  // TODO: type check
        if (option == 1) {
            // TODO: transcript
        } else if (option == 2) {
            // TODO: enroll
        } else if (option == 3) {
            // TODO: withdraw
        } else if (option == 4) {
            // TODO: personal details
        } else if (option == 5) {
            login(conn);
        } else {
            cout << "Invalid option. Please reselect." << endl;
        }
    }
}

void error(MYSQL* conn) {
    cout << "Could not execute statement(s).";
    mysql_close(conn);
    exit(0);
}
#pragma clang diagnostic pop