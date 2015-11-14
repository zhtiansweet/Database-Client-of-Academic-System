#include <mysql.h>
#include <iostream>
#include "utility.h"

using namespace std;

int main (int argc, const char * argv[]) {

    MYSQL *conn = initial();
    connect(conn);

    login(conn);

    close(conn);
}