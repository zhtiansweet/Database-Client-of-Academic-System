//
// Created by Di Tian on 11/13/15.
//
#include <mysql.h>
#include <iostream>
#include <termios.h>

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
void course_detail(string id, string course, string year, string quarter) {
    string query = "select uoscode, unitofstudy.uosname, semester, year, enrollment, maxenrollment, faculty.name, grade"
                   " from transcript join uosoffering using (Uoscode, Semester, Year)"
                   " join unitofstudy using (Uoscode)"
                   " join faculty on (instructorid = id)"
                   " where studid = " + id +
                   " and uoscode = \"" + course +
                   "\" and year = " + year +
                   " and semester = \"" + quarter + "\";";

    MYSQL_RES *res_set = send_query(query);

    int num_rows = (int) mysql_num_rows(res_set);
/*
    if(num_rows == 0) {
        cout << "Invalid combination of COURSE CODE, YEAR and QUARTER." << endl;
        return;
    }
*/
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

        unordered_set<string> available;

        int num_rows = (int) mysql_num_rows(res_set);
        for (int i = 0; i < num_rows; i++) {
            MYSQL_ROW row = mysql_fetch_row(res_set);
            string course = row[1];
            string quarter = row[2];
            string year = row[3];

            string combo = course + year + quarter;
            available.insert(combo);
            cout << row[1] << "  " << row[3] << "  " << row[2] << "  " << row[4] << endl;
        }

        mysql_free_result(res_set);

        string query1 = "select * from transcript where StudId = " + id +
                         " and grade is NULL order by year ASC, semester DESC;";
        MYSQL_RES * res_set1 = send_query(query1);
        int num_rows1 = (int) mysql_num_rows(res_set1);
        for (int i = 0; i < num_rows1; i++) {
            MYSQL_ROW row = mysql_fetch_row(res_set1);
            string course = row[1];
            string quarter = row[2];
            string year = row[3];

            string combo = course + year + quarter;
            available.insert(combo);
            cout << row[1] << "  " << row[3] << "  " << row[2] << "  " << "<<< Not Yet Graded >>>" << endl;
        }
        mysql_free_result(res_set1);

        cout << " ------------------------------------------" << endl;
        cout << "| You can proceed to the following options |" << endl;
        cout << " ------------------------------------------" << endl;
        cout << "1. Select a course to see details" << endl;
        cout << "2. Back to Student Menu" << endl;
        cout << "Please select: ";
        string option;
        cin >> option;

        if(option == "2") {
            return;
        } else if (option == "1") {
            string course;
            string year;
            string quarter;
            cout << "COURSE CODE: ";
            cin >> course;
            cout << "YEAR of the course: ";
            cin >> year;
            cout << "QUARTER of the course: ";
            cin >> quarter;

            string input_combo = course + year + quarter;
            for (int i = 0; i < input_combo.size(); ++i) {  // capitalize input_combo
                if (input_combo[i] >= 'a' && input_combo[i] <= 'z') {
                    input_combo[i] -= ('a' - 'A');
                }
            }

            auto search = available.find(input_combo);
            if (search == available.end()) {
                cout << endl << "Invalid combination of COURSE CODE, YEAR and QUARTER." << endl << endl;
            } else {
                course_detail(id, course, year, quarter);
            }
        } else {
            cout << endl << "Invalid option! Please reselect." << endl << endl;
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
    // cout << stmt_str << endl;
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

    unordered_set<string> available;
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
            string course = row[0];
            string year = row[2];
            string quarter = row[1];

            string combo = course + year + quarter;
            available.insert(combo);
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
            string year;
            string quarter;
            cout << "COURSE CODE: ";
            cin >> course;
            cout << "YEAR of the course: ";
            cin >> year;
            cout << "QUARTER of the course: ";
            cin >> quarter;

            string input_combo = course + year + quarter;
            for (int i = 0; i < input_combo.size(); ++i) {  // capitalize input_combo
                if (input_combo[i] >= 'a' && input_combo[i] <= 'z') {
                    input_combo[i] -= ('a' - 'A');
                }
            }

            auto search = available.find(input_combo);
            if (search == available.end()) {
                cout << endl << "Invalid combination of COURSE CODE, YEAR and QUARTER." << endl << endl;
            } else {

                string query1 = "select uoscode, semester, year"
                                        " from transcript join unitofstudy using (uoscode)"
                                        " where studid = " + id +
                                " and uoscode = \"" + course +
                                "\" and semester = \"" + quarter +
                                "\" and year = " + year +
                                " and grade is NULL order by year ASC, semester DESC;";

                MYSQL_RES *res_set1 = send_query(query1);

                MYSQL_ROW row = mysql_fetch_row(res_set1);
                course = row[0];
                quarter = row[1];
                year = row[2];
                mysql_free_result(res_set1);

                // call procedure to modify transcript table and # enrollment
                string query2 = "CALL withdraw(" + id + ", \"" + course + "\", " + year + ", \"" + quarter + "\"); ";

                MYSQL_RES *res_set2 = send_query(query2);
                MYSQL_ROW row1 = mysql_fetch_row(res_set2);

                if (row1[0] != nullptr) {
                    cout << endl << "WARNING: # enrollment of " << course <<
                    " is less than half of its max enrollment.";
                    cout << endl;
                }


                cout << endl << "You have successfully withdrawn [" << course << "] of [" << year << "], [";
                cout << quarter << "]." << endl << endl;

                cin.get();
                while (true) {
                    cout << "Press ENTER key to go back to \"Student Menu\"..." << endl;
                    if (cin.get() == '\n') {
                        student_menu(info);
                    }
                }
            }
        } else {
            cout << "Invalid option. Please reselect." << endl;
        }
    }
}


void personal_details(LoginInfo* info) {
    cout << " --------------------------------------------" << endl;
    cout << "| Personal Information (ID | Name | Address) |" << endl;
    cout << " --------------------------------------------" << endl;
    string id = info->GetId();  // id is guaranteed in the database
    string stmt_str = "select id, name, address from student where id = " + id + ";";
    MYSQL_RES* res_set = send_query(stmt_str);
    int num_rows = (int) mysql_num_rows(res_set);
    for (int i=0; i<num_rows; ++i) {
        MYSQL_ROW row = mysql_fetch_row(res_set);
        cout << row[0] << "  |  " << row[1] << "  |  " << row[2] << endl;
    }

    while (true) {
        cout << endl;
        cout << " ------------------------------------------" << endl;
        cout << "| You can proceed to the following options |" << endl;
        cout << " ----------------------------------------- " << endl;
        cout << "1. Reset password" << endl;
        cout << "2. Change address" << endl;
        cout << "3. Back to Student Menu" << endl;
        cout << "Please select: ";
        string option;
        cin >> option;
        if (option == "1") {
            cin.get();  // dump the new line feed which follows "1"
            cout << "Enter new password: ";
            string pwd0 = get_password();
            // cout << "-----" << pwd0 << ", the size is " << pwd0.size() << endl;
            cout << endl << "Confirm new password: ";
            string pwd1 = get_password();
            // cout << "+++++" << pwd1 << ", the size is " << pwd1.size() << endl;
            //cout << (pwd0 == pwd1);
            if (pwd0 == pwd1) {
                string stmt_str = "update student set password = \"" + pwd0 + "\" where id = " + id + ";";
                MYSQL *conn = initialize();
                connect(conn);
                mysql_query(conn, stmt_str.c_str());
                if ((long) mysql_affected_rows(conn) == 1) {
                    cout << endl << endl << "Password reset succeeded!" << endl;
                } else {
                    cout << endl << endl << "Password reset failed!" << endl;
                }
                close(conn);
            } else {
                cout << endl << endl << "Passwords did not match!" << endl;
            }
        } else if (option == "2") {
            cin.get();  // dump the new line feed which follows "1"
            cout << "Enter new address: ";
            char addr_input[60];
            cin.getline(addr_input, sizeof(addr_input));
            string addr = string(addr_input);
            string stmt_str = "update student set address = \"" + addr + "\" where id = " + id + ";";
            MYSQL *conn = initialize();
            connect(conn);
            mysql_query(conn, stmt_str.c_str());
            if ((long) mysql_affected_rows(conn) == 1) {
                cout << endl << "Address reset succeeded!" << endl;
            } else {
                cout << endl << "Address reset failed!" << endl;
            }
            close(conn);
            cout << " --------------------------------------------" << endl;
            cout << "| Personal Information (ID | Name | Address) |" << endl;
            cout << " --------------------------------------------" << endl;
            //string id = info->GetId();  // id is guaranteed in the database
            string stmt_str1 = "select id, name, address from student where id = " + id + ";";
            MYSQL_RES* res_set1 = send_query(stmt_str1);
            int num_rows1 = (int) mysql_num_rows(res_set1);
            for (int i=0; i<num_rows1; ++i) {
                MYSQL_ROW row = mysql_fetch_row(res_set1);
                cout << row[0] << "  |  " << row[1] << "  |  " << row[2] << endl;
            }
            mysql_free_result(res_set1);
        } else if (option == "3") {
            student_menu(info);
        } else {
            cout << endl << "Invalid option. Please reselect." << endl;
        }
    }
}

int getch() {
    struct termios oldt, newt;
    int ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}

string get_password() {
    string pwd;
    int c;
    while ( (c=getch()) != 10 ) {  // new line
        pwd.push_back(c);
    }
    return pwd;
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
        cout << endl;
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
                cout << endl;
            }
            cout << endl;
        }
        mysql_free_result(res_set);

        cout << " ------------------------------------------" << endl;
        cout << "| You can proceed to the following options |" << endl;
        cout << " ------------------------------------------" << endl;
        cout << "1. Transcript" << endl << "2. Enroll" << endl << "3. Withdraw" << endl
        << "4. Personal Details" << endl << "5. Logout" << endl << "6. Exit" << endl;
        cout << endl;
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
            personal_details(info);
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

bool check(string str) {
    for(int i = 0; i < str.size(); i++) {
        if(str[i] < '0' || str[i] > '9') {
            return false;
        }
    }
    return true;
}

void login() {

    while (true) {
        string student_id;
        string password;
        cout << "Username: ";
        cin >> student_id;

        if(!check(student_id)) {
            cout << "Username should only contain numbers!" << endl << endl;
            continue;
        }

        cout << "Password: ";
        // cin >> password;
        cin.get();  // dump the new line feed character
        password = get_password();


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
            } else {
                cout << "Wrong password" << endl;
            }
        }
        mysql_free_result(res_set);

        delete info;
    }
}

#pragma clang diagnostic pop