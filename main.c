#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <mysql.h>
#include "include/main.h"

// Flag used by handler - changed to 0 when user presses Ctrl-C
// Loop that reads & records temperatures keeps running when
// keepRunning = 1
int8_t volatile keepRunning = 1;


extern int connectDb(MYSQL *conn);
extern int writeThemp(MYSQL *conn, struct ds18b20 *d);


// Find connected 1-wire devices. 1-wire driver creates entries for each device
// in /sys/bus/w1/devices on the Raspberry Pi. Create linked list.
int8_t findDevices(struct ds18b20 *d) {
  	DIR *dir;
    struct dirent *dirent;
  	
  	struct ds18b20 *newDev;
    //char path[] = "/sys/bus/w1/devices";
    char path[] = "/tmp/bus/w1/devices";   // для отладки без устройств 
    int8_t i = 0;
    
    dir = opendir(path);
	if (dir != NULL){
		while ((dirent = readdir(dir))) {
			// Файл должен быть ссылкой. Имя группы 28 старое, 10 новое
	        // if (dirent->d_type == DT_LNK && strstr(dirent->d_name, "28-") != NULL) {
			if ( (strstr(dirent->d_name, "28-") != NULL) || (strstr(dirent->d_name, "10-") != NULL)) {
				newDev = malloc( sizeof(struct ds18b20) );
	            strcpy(newDev->devID, dirent->d_name);
	            // Assemble path to OneWire device
	            sprintf(newDev->devPath, "%s/%s/w1_slave", path, newDev->devID);
	            i++;
				newDev->next = 0;
				d->next = newDev;
				d = d->next;
	        }
		}
		(void) closedir(dir);
	
	}else {
		perror ("Couldn't open the w1 devices directory");
		return 1;
	}
	return i;
}


// Cycle through linked list of devices & take readings.
// Print out results & store readings in DB.
int8_t readTemp(struct ds18b20 *d, MYSQL* conn) {
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

// Called when user presses Ctrl-C
void intHandler() {
    printf("\nОстановка...\n");
    keepRunning = 0;
}

int main (int argc, char **argv) {
 	// Intercept Ctrl-C (SIGINT) in order to finish writing data & close DB
	signal(SIGINT, intHandler);

	struct ds18b20 *rootNode;
	struct ds18b20 *devNode;

	MYSQL *conn = mysql_init(NULL);

	connectDb(conn);

	// Handler sets keepRunning to 0 when user presses Ctrl-C
	// When Ctrl-C is pressed, complete current cycle of readings,
	// close DB, & exit.
	while(keepRunning) {
		rootNode = malloc( sizeof(struct ds18b20) );
		devNode = rootNode;
		// Создаем связанный список всех устройств
		int8_t devCnt = findDevices(devNode);
		printf("\nНайдено %d устройства\n\n", devCnt);
		
		// Считываем информацию по каждому
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

		sleep(3);
	}
	mysql_close(conn);
	return 0;
}

