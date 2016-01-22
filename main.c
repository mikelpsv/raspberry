/* main.c
// 
// Author: Mike Lapshov
// Copyright (C) 2011-2016 Mike Lapshov
*/

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <mysql.h>
#include <getopt.h>
#include "include/main.h"



// Флаг для обработки события нажатия Ctrl-C
// keepRunning = 1
int8_t volatile keepRunning = 1;


// >> Обработка параметров 

extern char *optarg;
extern int optind, opterr, optopt;

struct globalArgs_t {
	int delay_w1;		// -d option
	char *path_w1;		// -p
  	char *server;		// -S
   	char *user;			// -U
   	char *password;		// -P
   	char *database;		// -D
} globalArgs;

static const char *optString = "d:p:h?S:U:P:D:";

static const struct option longOpts[] = {
	{ "help", no_argument, NULL, 'h' },
	{ "delay_w1", required_argument, NULL, 'd' },
	{ "path_w1", required_argument, NULL, 'p' },
	{ "server_mysql", required_argument, NULL, 'S' },
	{ "user_mysql", required_argument, NULL, 'U' },
	{ "pass_mysql", required_argument, NULL, 'P' },
	{ "database", required_argument, NULL, 'D' },
	{ NULL, no_argument, NULL, 0 }
};

// << Обработка параметров 

extern int connectDb(MYSQL *conn, char *server, char *user, char *password, char *database);
extern int writeThemp(MYSQL *conn, struct ds18b20 *d);




// Обработчик прерывания Ctrl+C
void intHandler();
// Поиск устройств 1-ware. Создает связвнный список структур
int findDevices(struct ds18b20 *d);
// Считывает данные с устройств по списку. Выводит на экран и записывает в БД
int readTemp(struct ds18b20 *d, MYSQL* conn);
// Вывод сообщения
void displayUsage( void );


int main (int argc, char **argv) {
 	// Intercept Ctrl-C (SIGINT) in order to finish writing data & close DB
	signal(SIGINT, intHandler);


	int opt 			= 0;
	int longIndex 		= 0;

	
	globalArgs.delay_w1 	= DEF_DELAY_W1;
	globalArgs.path_w1 		= DEF_PATH_DEV_W1;
	globalArgs.server    	= DEF_MYSQL_SERVER;
   	globalArgs.user      	= DEF_MYSQL_USER;
   	globalArgs.password  	= "";
   	globalArgs.database  	= DEF_MYSQL_DB;	


	/* Process the arguments with getopt_long(), then 
	 * populate globalArgs. 
	 */
	opt = getopt_long( argc, argv, optString, longOpts, &longIndex );
	while( opt != -1 ) {
		switch( opt ) {
			case 'd':
				if(atoi(optarg) == 0){
					printf("Некорректное значение параметра: %c = %s\n", opt, optarg);
					displayUsage();
				}else{
					globalArgs.delay_w1 = atoi(optarg);	/* true */
				}
				break;

			case 'S':
				globalArgs.server = optarg;
				break;
			case 'U':
				globalArgs.user = optarg;
				break;
			case 'P':
				globalArgs.password = optarg;
				break;
			case 'D':
				globalArgs.database = optarg;
				break;

			case 'h':
			case '?':
				displayUsage();
			break;
			default:
				break;
		}
		
		opt = getopt_long( argc, argv, optString, longOpts, &longIndex );
	}


	struct ds18b20 *rootNode;
	struct ds18b20 *devNode;

	MYSQL *conn = mysql_init(NULL);
	
	if(!connectDb(conn, globalArgs.server, globalArgs.user, globalArgs.password, globalArgs.database)){
		printf("Ошибка подключения к MySQL серверу: %s\n", globalArgs.server);
	}

	// Handler sets keepRunning to 0 when user presses Ctrl-C
	// When Ctrl-C is pressed, complete current cycle of readings,
	// close DB, & exit.
	while(keepRunning) {
		rootNode = malloc( sizeof(struct ds18b20) );
		devNode = rootNode;
		// Создаем связанный список всех устройств
		int8_t devCnt = findDevices(devNode);
		printf("\nНайдено %d устройств:\n", devCnt);
		
		// Считываем информацию по каждому датчику
		readTemp(rootNode, conn);
		
		// Удаляем список
		while(rootNode) {
	  		// Start with current value of root node
	  		devNode = rootNode;
	  		// Save address of next devNode to rootNode before 
	        // deleting current devNode
	  		rootNode = devNode->next;
	  		// Free current devNode.
	  		free(devNode);
		}
		// Now free rootNode
		free(rootNode);

		sleep(globalArgs.delay_w1);
	}
	mysql_close(conn);
	
	return EXIT_SUCCESS;
}


// Поиск устройств 1-ware. Создает связвнный список структур
int findDevices(struct ds18b20 *d) {
  	DIR *dir;
    struct dirent *dirent;
  	
  	struct ds18b20 *newDev;

    int8_t i = 0;
    
    dir = opendir(globalArgs.path_w1);
	if (dir != NULL){
		while ((dirent = readdir(dir))) {
			// Файл должен быть ссылкой. Имя группы 28 старое, 10 новое
	        // if (dirent->d_type == DT_LNK && strstr(dirent->d_name, "28-") != NULL) {
			if ( (strstr(dirent->d_name, "28-") != NULL) || (strstr(dirent->d_name, "10-") != NULL)) {
				newDev = malloc( sizeof(struct ds18b20) );
	            strcpy(newDev->devID, dirent->d_name);
	            // Assemble path to OneWire device
	            sprintf(newDev->devPath, "%s/%s/w1_slave", globalArgs.path_w1, newDev->devID);
	            i++;
				newDev->next = 0;
				d->next = newDev;
				d = d->next;
	        }
		}
		(void) closedir(dir);
	
	}else {
		perror ("Невозможно открыть каталог w1 устройств");
		return 1;
	}
	return i;
}


// Считывает данные с устройств по списку. Выводит на экран и записывает в БД
int readTemp(struct ds18b20 *d, MYSQL* conn) {
	while(d->next != NULL){
    	d = d->next;
    	int fd = open(d->devPath, O_RDONLY);
    	if(fd == -1) {
   			printf("Невозможно открыть w1 устройство: %s\n", d->devPath);
    	}
  		
  		// 1-wire driver stores data in file as long block of text
        // Store file contents in buf & look for t= that marks start of temp.
    	char buf[256];
    	ssize_t numRead;
        while((numRead = read(fd, buf, 256)) > 0) {
        	strncpy(d->tempData, strstr(buf, "t=") + 2, 5);
            double tempC = strtod(d->tempData, NULL);
   			// Driver stores temperature in units of .001 degree C
   			tempC /= 1000;
            printf("Устройство: %s  - ", d->devID);
            printf("Темп.: %.3f C  \n", tempC);
   			writeThemp(conn, d);	
        }
        close(fd);
  	}
  	
  	return 0;
}

void intHandler() {
    printf("\nОстановка...\n");
    keepRunning = 0;
}

void displayUsage( void )
{
	puts("Использование:\n");
	puts(" smart_sens [option]\n");

	puts("Параметры запуска:\n");
	puts(" -d --delay_w1		- интервал опроса температурных датчиков 1-wire. По-умолчанию:");
	puts(" -S --server_mysql	- адрес MySQL сервера");
	puts(" -U --user_mysql	- пользователь MySQL сервера");
	puts(" -P --password_mysql	- пароль пользователя MySQL сервера");
	puts("\n");
	exit( EXIT_FAILURE );
}



