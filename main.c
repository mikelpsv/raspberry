#include "mysql.h"
#include <stdio.h>
#include <time.h>
#include <unistd.h>

int db_connected = 0;

int main(int argc, char const *argv[])
{

  	MYSQL *conn;
   	MYSQL_RES *res;
   	MYSQL_ROW row;

   	// Данные подключения к MySql
   	char *server = "localhost";
   	char *user = "root";
   	char *password = "password";
   	char *database = "db";
   	
	char path[] = "/sys/bus/w1/devices"; // Путь к датчикам



   	conn = mysql_init(NULL);

	time_t t;
	struct tm *tm;
	char s[64];

   	/* Connect to database */
   	if (!mysql_real_connect(conn, server,
        user, password, database, 0, NULL, 0)) {
    	fprintf(stderr, "%s\n", mysql_error(conn));
      	exit(1);
    }else{
    	db_connected = 1;

    	printf("%s\n", "Connected..");
    	while(1){
    		sleep(5);

			t = time(NULL);
			tm = localtime(&t);
			strftime(s, sizeof(s), "%c", tm);
			printf("strftime = %s\n", s);

    		printf("%s %d\n", "Опрос датчиков...", db_connected);
    	}
    }

	return 0;
}

int db_connect(){


}

int readThempSensor(char *sensor_name){
	sprintf(devPath, "%s/%s/w1_slave", path, dev);
	
	FILE *file;
	file = fopen( file_name, "r" );
}


