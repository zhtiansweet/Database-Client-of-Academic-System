//
// Created by Di Tian on 11/13/15.
//
#include <mysql.h>
#include <iostream>
#include "utility.h"

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
        cout << "Username: ";
        cin >> student_id;
        cout << "Password: ";
        cin >> password;

        MYSQL_RES *res_set;
        MYSQL_ROW row;

        int status = mysql_query(conn, ("select * from student where id = " + student_id).c_str());
        if (status) {
            cout << "Could not execute statement(s).";
            mysql_close(conn);
            exit(0);
        }

        res_set = mysql_store_result(conn);
        int num_rows = (int) mysql_num_rows(res_set);

        if (num_rows == 0) {
            cout << "Student doesn't exist!" << endl;
        } else {
            row = mysql_fetch_row(res_set);
            if (row[2] == password) {
                cout << "Successfully log in!" << endl;
                break;
            } else {
                cout << "Wrong password:(" << endl;
            }
        }
        mysql_free_result(res_set);
    }
}

