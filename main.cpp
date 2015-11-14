#include <mysql.h>
#include <iostream>
using namespace std;
int main (int argc, const char * argv[]) {
    MYSQL *conn;
    conn = mysql_init(NULL);
    if (mysql_real_connect(conn, "localhost", "root", "12345",
                           "project3-nudb", 0, 0, 0) == NULL) {
        //unable to connect
        cout << "Oh Noes!\n";
    }
    else {
        cout << "You are now connected. Welcome!\n";
    }

    while(true) {
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
            cout << "Could not execute statement(s)";
            mysql_close(conn);
            exit(0);
        }

        res_set = mysql_store_result(conn);
        int numrows = (int) mysql_num_rows(res_set);

        if (numrows == 0) {
            cout << "Student doesn't exists!\n";
        } else {
            row = mysql_fetch_row(res_set);
            if (row[2] == password) {
                cout << "Successfully log in!\n";
                break;
            } else {
                cout << "Wrong password:(\n";
            }
        }
        mysql_free_result(res_set);
    }

    mysql_close(conn);

    cout << "dog bro" << endl;
}