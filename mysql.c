#include <string.h>
#include <stdlib.h>
#include <mysql.h>
#include "include/main.h"



int connectDb(MYSQL *conn){

   	char *server    = "localhost";
   	char *user      = "root";
   	char *password  = "password";
   	char *database  = "smarthome";

	  if(!mysql_real_connect(conn, server,
        user, password, database, 0, NULL, 0))
        return 0;

    return 1;
}

void getVersionDb(MYSQL *conn, char *ver){
  if(mysql_query(conn, "SELECT VERSION()") != 0)
       perror("Error: can't execute SQL-query\n");

    // Получаем дескриптор результирующей таблицы
    MYSQL_RES *res = mysql_store_result(conn);
    if(res == NULL){
      perror("Error: can't get the result description\n");
    }

    // Получаем первую строку из результирующей таблицы
    MYSQL_ROW row = mysql_fetch_row(res);
    if(mysql_errno(conn) > 0) 
      perror("Error: can't fetch result\n");

    strcpy(ver, row[0]);

    // Освобождаем память, занятую результирующей таблицей
    mysql_free_result(res);
}


int writeThemp(MYSQL *conn, struct ds18b20 *d){
    MYSQL_STMT *sql_statement1;
    MYSQL_BIND sql_bind_parameters1[2];
    int sql_error = 0;


    //Setup to create query
    sql_statement1= mysql_stmt_init(conn);
    if (!sql_statement1)
      sql_error = 1;


    //----- SET THE QUERY TEXT -----
    #define SQL_QUERY_TEXT_1 "INSERT INTO `smarthome`.`sensor_data` (`date`, `dev_id`, `value`) VALUES (NOW(), ?, ?);"
    
    if (mysql_stmt_prepare(sql_statement1, SQL_QUERY_TEXT_1, strlen(SQL_QUERY_TEXT_1)))
        sql_error = 1;

    //----- SET THE QUERY PARAMETER VALUES -----
    //If you want to know how many parameters are expected
    //int param_count = mysql_stmt_param_count(sql_statement1);

    //Set the parameter values
    memset(sql_bind_parameters1, 0, sizeof(sql_bind_parameters1));    //Reset the parameters memory to null

    //Integer
    //int int_data = 10;
    //sql_bind_parameters1[0].buffer_type = MYSQL_TYPE_LONG;
    //sql_bind_parameters1[0].buffer = (char*)&d->DevId;    //<Note: this is a pointer!
    //sql_bind_parameters1[0].is_null = 0;
    //sql_bind_parameters1[0].length = 0;


    unsigned long str_length = strlen(d->devID);
  
    sql_bind_parameters1[0].buffer_type = MYSQL_TYPE_STRING;
    sql_bind_parameters1[0].buffer = (char*)&d->devID; //<Note: this is a pointer!
    sql_bind_parameters1[0].buffer_length = sizeof(&d->devID);
    sql_bind_parameters1[0].is_null = 0;
    sql_bind_parameters1[0].length = &str_length;

    
    double tempC = strtod(d->tempData, NULL);
    tempC /= 1000;

    sql_bind_parameters1[1].buffer_type = MYSQL_TYPE_DOUBLE;
    sql_bind_parameters1[1].buffer = (char*)&tempC;    //<Note: this is a pointer!
    sql_bind_parameters1[1].is_null = 0;
    sql_bind_parameters1[1].length = 0;

/*
    //string
    char str_data[50] = "Hello";
    unsigned long str_length = strlen(str_data);
    sql_bind_parameters1[1].buffer_type = MYSQL_TYPE_STRING;
    sql_bind_parameters1[1].buffer = (char*)str_data;
    sql_bind_parameters1[1].buffer_length = sizeof(str_data);
    sql_bind_parameters1[1].is_null = 0;
    sql_bind_parameters1[1].length = &str_length;     //<Note: this is a pointer!

    //smallint
    short small_data;
    my_bool is_null = 1;    //We'll store this as null in this example
    sql_bind_parameters1[2].buffer_type = MYSQL_TYPE_SHORT;
    sql_bind_parameters1[2].buffer = (char*)&small_data;  //<Note: this is a pointer!
    sql_bind_parameters1[2].is_null = &is_null;       //<Note: this is a pointer!
    sql_bind_parameters1[2].length = 0;
*/

    //Pointers are used in the bind parameters so that if you are say adding multiple rows you can use the same query setup with new values for each execute of it.

    //Bind the buffers
    if (mysql_stmt_bind_param(sql_statement1, sql_bind_parameters1))
      sql_error = 1;

    //----- EXECUTE THE QUERY ------
    if (!sql_error)
    {
      if (mysql_stmt_execute(sql_statement1))
        sql_error = 1;
    }

    //If you want to get the number of affected rows
    //my_ulonglong affected_rows = mysql_stmt_affected_rows(sql_statement1);
    //if (affected_rows != 1)
    //{
    //  do something
    //}

    //IF YOU WANT TO GET THE VALUE GENERATED FOR AN AUTO_INCREMENT COLUMN IN THE PREVIOUS INSERT/UPDATE STATEMENT
    //my_ulonglong sql_insert_id = mysql_stmt_insert_id(sql_statement1);

    //If you want to do the query again then change any values you want to change and call mysql_stmt_execute(sql_statement1) again

    //Close the statement
    if (sql_statement1)
    {
      if (mysql_stmt_close(sql_statement1))
        sql_error = 1;
    }


}




