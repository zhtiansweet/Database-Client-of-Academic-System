//
// Created by Di Tian on 11/13/15.
//

#ifndef DATABASE_UTILITY_H
#define DATABASE_UTILITY_H

#include <mysql.h>
#include "login_information.h"
/*
MYSQL* initialize();
void connect(MYSQL* conn);
void close(MYSQL* conn);
*/
void login();

void student_menu(LoginInfo* info);

string get_password();
/*
void student_menu(MYSQL* conn, LoginInfo* info);

void enroll(MYSQL* conn, LoginInfo* info);

void transcript(MYSQL* conn, string id);

void error(MYSQL* conn);
 */
#endif //DATABASE_UTILITY_H
