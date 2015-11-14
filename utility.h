//
// Created by Di Tian on 11/13/15.
//

#ifndef DATABASE_UTILITY_H
#define DATABASE_UTILITY_H

#include <mysql.h>

MYSQL* initial();
void connect(MYSQL* conn);
void close(MYSQL* conn);

void login(MYSQL* conn);


#endif //DATABASE_UTILITY_H
