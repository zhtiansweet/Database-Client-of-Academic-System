//
// Created by Di Tian on 11/13/15.
//
#include <mysql.h>
#include <iostream>

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
        cout << "Connection failed!" << endl;
        exit(0);
    }
}

void close(MYSQL* conn) {
    if (conn == nullptr) {
        cout << "Null pointer." << endl;
        exit(0);
    }
    mysql_close(conn);
}

void error(MYSQL* conn) {
    cout << "Could not execute statement(s).";
    mysql_close(conn);
    exit(0);
}

// Connect to MySQL, send query, return result
MYSQL_RES* send_query(string query) {
    MYSQL* conn = initialize();
    connect(conn);
    if (mysql_query(conn, query.c_str())) {
        error(conn);
    }

    MYSQL_RES* res_set = mysql_store_result(conn);
    close(conn);
    return res_set;
}

// Triggered in transcript module
void course_detail(string id, string course) {
    string query = "select uoscode, unitofstudy.uosname, semester, year, enrollment, maxenrollment, faculty.name, grade"
                   " from transcript join uosoffering using (Uoscode, Semester, Year)"
                   " join unitofstudy using (Uoscode)"
                   " join faculty on (instructorid = id)"
                   " where studid = " + id + " and uoscode = \"" + course +
                   "\" order by year, semester;";

    MYSQL_RES *res_set = send_query(query);

    int num_rows = (int) mysql_num_rows(res_set);
    for(int i = 0; i < num_rows; i++) {
        MYSQL_ROW row = mysql_fetch_row(res_set);
        cout << endl;
        cout << "Course: " << row[0] << "   " << row[1] << endl;
        cout << "Quarter: " << row[3] << " " << row[2] << endl;
        cout << "# Enrolled/Max: " << row[4] << "/" << row[5] << endl;
        cout << "Lecturer: " << row[6] << endl;
        cout << "Grade: ";
        if (row[7] == nullptr) {
            cout << "<<< Not Yet Graded >>>" << endl;
        } else {
            cout << row[7] << endl;
        }
        cout << endl;
    }

    mysql_free_result(res_set);

    cin.get();
    while (true) {
        cout << "Press ENTER key to go back to \"Unofficial Transcript\"..." << endl;
        if (cin.get() == '\n') {
            return;
        }
    }
}

void transcript(LoginInfo* info) {
    string id = info->GetId();
    while(true) {
        cout << " -----------------------" << endl;
        cout << "| Unofficial Transcript |" << endl;
        cout << " -----------------------" << endl;

        string query = "select * from transcript where StudId = " + id +
                        " and grade is not NULL order by year ASC, semester DESC;";
        MYSQL_RES *res_set = send_query(query);

        int num_rows = (int) mysql_num_rows(res_set);
        for (int i = 0; i < num_rows; i++) {
            MYSQL_ROW row = mysql_fetch_row(res_set);
            cout << row[1] << "  " << row[2] << "  " << row[3] << "  " << row[4] << endl;
        }

        mysql_free_result(res_set);

        string query1 = "select * from transcript where StudId = " + id +
                         " and grade is NULL order by year ASC, semester DESC;";
        MYSQL_RES * res_set1 = send_query(query1);
        int num_rows1 = (int) mysql_num_rows(res_set1);
        for (int i = 0; i < num_rows1; i++) {
            MYSQL_ROW row = mysql_fetch_row(res_set1);
            cout << row[1] << "  " << row[2] << "  " << row[3] << "  " << "<<< Not Yet Graded >>>" << endl;
        }
        mysql_free_result(res_set1);

        cout << endl << "Type any course number above to view course details;" << endl;
        cout << "Or type \"0\" to go back to \"Student Menu\"." << endl;

        string course;
        cin >> course;  //TODO: 1. type check; 2. non-exist course number
        if (course == "0") {
            return;
        } else {
            course_detail(id, course);
        }
    }
}

void enroll(LoginInfo* info) {
    string id = info->GetId();
    int cur_q_year = info->GetCurrentQuarterPtr()->GetQuarter_SchoolYear();
    string cur_q_name = info->GetCurrentQuarterPtr()->GetQuarter_Name();
    int next_q_year = info->GetNextQuarterPtr()->GetQuarter_SchoolYear();
    string next_q_name = info->GetNextQuarterPtr()->GetQuarter_Name();

    unordered_set<string> available;

    string stmt_str = "CALL candidate_course(" + id + ", " + to_string(cur_q_year) + ", \"" + cur_q_name + "\", " +
                  to_string(next_q_year) + ", \"" + next_q_name + "\");";
    MYSQL_RES* res_set = send_query(stmt_str);
    int num_rows = (int) mysql_num_rows(res_set);
    if (num_rows == 0) {
        cout << "Yeah! You are not eligible to enroll in any course." << endl;
    } else {
        cout << " ----------------------------------------------------------" << endl;
        cout << "| You might be eligible to enroll in the following courses |" << endl;
        cout << " ----------------------------------------------------------" << endl;
        for (int i=0; i<num_rows; ++i) {
            MYSQL_ROW row = mysql_fetch_row(res_set);
            string c_name = string(row[0]);
            string c_year = string(row[2]);
            string c_qatr = string(row[3]);
            string combo = c_name + c_year + c_qatr;
            available.insert(combo);
            cout << row[0] <<  "  " << row[2] << "  " << row[3] << "  " << row[1] << endl;
        }
        cout << endl;
    }
    mysql_free_result(res_set);

    while (true) {
        cout << " ------------------------------------------" << endl;
        cout << "| You can proceed to the following options |" << endl;
        cout << " ------------------------------------------" << endl;
        cout << "1. Select a course" << endl;
        cout << "2. Back to Student Menu" << endl;
        cout << "Please select: ";
        string option;
        cin >> option;
        if (option == "1") {
            string new_course;
            string new_course_year;
            string new_course_quarter;
            cout << "COURSE CODE: ";
            cin >> new_course;
            cout << "YEAR of the new course: ";
            cin >> new_course_year;
            cout << "QUARTER of the new course: ";
            cin >> new_course_quarter;

            string input_combo = new_course + new_course_year + new_course_quarter;
            for (int i = 0; i < input_combo.size(); ++i) {  // capitalize input_combo
                if (input_combo[i] >= 'a' && input_combo[i] <= 'z') {
                    input_combo[i] -= ('a' - 'A');
                }
            }

            auto search = available.find(input_combo);
            if (search == available.end()) {
                cout << endl << "Invalid combination of COURSE CODE, YEAR and QUARTER." << endl << endl;
            } else {
                string _c_name = input_combo.substr(0, 8);
                string _c_year = input_combo.substr(8, 4);
                string _c_quarter = input_combo.substr(12, 2);
                string _c_begins_on = _c_year + "-";
                if (_c_quarter == "Q1") {
                    _c_begins_on += "09-15";
                } else if (_c_quarter == "Q2") {
                    _c_begins_on += "01-05";
                } else if (_c_quarter == "Q3") {
                    _c_begins_on += "03-30";
                } else if (_c_quarter == "Q4") {
                    _c_begins_on += "06-22";
                } else {
                    _c_begins_on += "12-31";
                }

                string stmt_str_1 = "CALL enroll(" + id + ", \"" + _c_name + "\", " + _c_year + ", \"" + _c_quarter +
                                    "\", \"" + _c_begins_on + "\");";
                // cout << stmt_str_1 << endl;
                MYSQL_RES *res_set_1 = send_query(stmt_str_1);
                int num_rows_1 = (int) mysql_num_rows(res_set_1);
                if (num_rows_1 == 0) {
                    cout << endl << "You have successfully enrolled in [" << _c_name << "] of year [" <<
                    _c_year << "], quarter [" << _c_quarter << "]." << endl << endl;
                } else {
                    cout << endl;
                    cout << " -------------------------------------------------------------------" << endl;
                    cout << "| You have to clear the following prerequisite(s) of " << _c_name << " first |" << endl;
                    cout << " -------------------------------------------------------------------" << endl;
                    for (int i = 0; i < num_rows_1; ++i) {
                        MYSQL_ROW row = mysql_fetch_row(res_set_1);
                        cout << row[0] << "  " << row[1] << endl;
                    }
                    cout << endl;
                }
                mysql_free_result(res_set_1);

                cin.get();
                while (true) {
                    cout << "Press ENTER key to go back to \"Student Menu\"..." << endl;
                    if (cin.get() == '\n') {
                        student_menu(info);
                    }
                }

            }
        } else if (option == "2") {
            student_menu(info);
        } else {
            cout << "Invalid option. Please reselect." << endl;
        }
    }
}

void withdraw(LoginInfo* info) {
    string id = info->GetId();
    string query = "select uoscode, semester, year, uosname"
                           " from transcript join unitofstudy using (uoscode)"
                           " where studid = " + id +
                   " and grade is NULL order by year ASC, semester DESC;";

    MYSQL_RES* res_set = send_query(query);
    int num_rows = (int) mysql_num_rows(res_set);
    if (num_rows == 0) {
        cout << "You don't have any course to withdraw!" << endl;
    } else {
        cout << " ----------------------------------------------------" << endl;
        cout << "| You are eligible to withdraw the following courses |" << endl;
        cout << " ----------------------------------------------------" << endl;
        for (int i=0; i<num_rows; ++i) {
            MYSQL_ROW row = mysql_fetch_row(res_set);
            cout << row[0] <<  " " << row[2] << " " << row[1] << " " << row[3] << endl;
        }
        cout << endl;
    }
    mysql_free_result(res_set);

    while(true) {
        cout << " ------------------------------------------" << endl;
        cout << "| You can proceed to the following options |" << endl;
        cout << " ------------------------------------------" << endl;
        cout << "1. Select a course" << endl;
        cout << "2. Back to Student Menu" << endl;
        cout << "Please select: ";
        string option;
        cin >> option;

        if (option == "2") {
            return;
        } else if (option == "1") {
            string course;
            cout << "COURSE CODE: ";
            cin >> course;

            // check whether course code is valid
            string query1 = "select uoscode, semester, year"
                            " from transcript join unitofstudy using (uoscode)"
                            " where studid = " + id +
                            " and uoscode = \"" + course +
                            "\" and grade is NULL order by year ASC, semester DESC;";
            //TODO: duplicate course?
            MYSQL_RES* res_set1 = send_query(query1);
            int num_rows1 = (int) mysql_num_rows(res_set1);
            if (num_rows1 == 0) {
                cout << "Invalid COURSE CODE!" << endl;
                continue;
            }
            MYSQL_ROW row = mysql_fetch_row(res_set1);
            string quarter = row[1];
            string year = row[2];
            mysql_free_result(res_set1);

            // call procedure to modify transcript table and # enrollment
            string query2 = "CALL withdraw(" + id + ", \"" + course + "\", " + year + ", \"" + quarter + "\");";

            MYSQL* conn = initialize();
            connect(conn);
            if (mysql_query(conn, query2.c_str())) {
                error(conn);
            }
            const char *trigger = mysql_sqlstate(conn);
            if(trigger == "1000") { //TODO: catch warning thrown by trigger
                cout << endl << "WARNING: # enrollment of " << course << " is less than half of its max enrollment.";
                cout << endl;
            }

            cout << endl << "You have successfully withdrawed " << course << endl << endl;
            close(conn);
            cin.get();
            while (true) {
                cout << "Press ENTER key to go back to \"Student Menu\"..." << endl;
                if (cin.get() == '\n') {
                    student_menu(info);
                }
            }
        } else {
            cout << "Invalid option. Please reselect." << endl;
        }
    }
}

void student_menu(LoginInfo* info) {
    int month = info->GetMonth();
    int day = info->GetDay();
    int year = info->GetYear();
    string weekday_name = info->GetNameOfWeekDay();

    string id = info->GetId();
    int school_year = info->GetCurrentQuarterPtr()->GetQuarter_SchoolYear();
    string quarter = info->GetCurrentQuarterPtr()->GetQuarter_Name();

    while (true) {
        cout << " --------------" << endl;
        cout << "| Student Menu |" << endl;
        cout << " --------------" << endl;
        cout << "You are logged in as " <<  id << "." << endl;
        cout << "Today is " << month << "-" << day << "-" << year << ", " << weekday_name << "." << endl << endl;
        string stmt_str = "select unitofstudy.UoSCode, unitofstudy.UoSName, transcript.Grade"
                " from student"
                " join transcript on student.Id = transcript.StudId"
                " join uosoffering on transcript.UoSCode = uosoffering.UoSCode and"
                " transcript.Semester = uosoffering.Semester and"
                " transcript.Year = uosoffering.Year"
                " join unitofstudy on transcript.UoSCode = unitofstudy.UoSCode";
        //" where student.Id = \"%s\" and uosoffering.Year = %d and uosoffering.Semester = \"%s\";";
        stmt_str += " where student.Id = " + id +
                    " and uosoffering.Year = " + to_string(school_year) +
                    " and uosoffering.Semester = \"" + quarter + "\";";

        MYSQL_RES* res_set = send_query(stmt_str);
        int num_rows = (int) mysql_num_rows(res_set);
        if (num_rows == 0) {
            cout << "Yeah! You are not enrolled in any course!" << endl;
        } else {
            cout << " -----------------------------------------------------" << endl;
            cout << "| You are currently enrolled in the following courses |" << endl;
            cout << " -----------------------------------------------------" << endl;
            for (int i=0; i<num_rows; ++i) {
                MYSQL_ROW row = mysql_fetch_row(res_set);
                cout << row[0] <<  "    " << row[1];
                /*
                if (row[2] == nullptr) {
                    cout << "    <<< Not Yet Graded >>>";
                }
                */
                cout << endl;
            }
            cout << endl;
            //cout << "--------------------------------------------" << endl;
        }
        mysql_free_result(res_set);

        cout << " ------------------------------------------" << endl;
        cout << "| You can proceed to the following options |" << endl;
        cout << " ------------------------------------------" << endl;
        cout << "1. Transcript" << endl << "2. Enroll" << endl << "3. Withdraw" << endl
        << "4. Personal Details" << endl << "5. Logout" << endl << "6. Exit" << endl;
        cout << endl;
        //cout << "--------------------------------------------" << endl;
        cout << "Please select: ";
        string option;
        cin >> option;
        if (option == "1") {
            transcript(info);
        } else if (option == "2") {
            enroll(info);
        } else if (option == "3") {
            withdraw(info);
        } else if (option == "4") {
            // TODO: personal details
        } else if (option == "5") {
            cout << "Bye!" << endl << endl;
            delete info;  // deallocate the LoginInfo object assigned to current user
            login();
        } else if (option == "6") {
            cout << "Bye!" << endl;
            delete info;
            exit(0);
        } else {
            cout << "Invalid option. Please reselect." << endl;
        }
    }
}

void login() {

    while (true) {
        string student_id;
        string password;
        cout << "Username: ";  // TODO: 1). int type check; 2). int length check.
        cin >> student_id;
        cout << "Password: ";
        cin >> password;

        LoginInfo* info = new LoginInfo(student_id);

        string query = "select * from student where id = " + student_id;
        MYSQL_RES *res_set = send_query(query);
        MYSQL_ROW row;
        int num_rows = (int) mysql_num_rows(res_set);

        if (num_rows == 0) {
            cout << "Student does not exist." << endl;
        } else {
            row = mysql_fetch_row(res_set);
            if (row[2] == password) {
                student_menu(info);
                break;
            } else {
                cout << "Wrong password." << endl;
            }
        }
        mysql_free_result(res_set);

        delete info;
    }
}

#pragma clang diagnostic pop