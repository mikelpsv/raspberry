#include "mysql.h"


int connectDb(){
  	MYSQL *conn;
   	MYSQL_RES *res;
   	MYSQL_ROW row;

   	char *server = "localhost";
   	char *user = "root";
   	char *password = "password";
   	char *database = "db";

	conn = mysql_init(NULL);

	mysql_real_connect(conn, server,
        user, password, database, 0, NULL, 0)
}

int writeTemp(){

}