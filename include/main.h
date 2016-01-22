#ifndef __MAIN_H__
#define __MAIN_H__

#define DEF_DELAY_W1 3
#define DEF_PATH_DEV_W1 	"/tmp/bus/w1/devices"
#define DEF_MYSQL_SERVER 	"localhost"
#define DEF_MYSQL_USER 		"root"
#define DEF_MYSQL_DB 		"smarthome"

// struct to hold ds18b20 data for linked list
// 1-Wire driver stores info in file for device as text
struct ds18b20 {
	char 	devPath[128];
	char 	devID[16];
	char 	tempData[6];
	struct 	ds18b20 *next;
};


#endif
