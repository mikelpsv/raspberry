#ifndef __MAIN_H__
#define __MAIN_H__



// struct to hold ds18b20 data for linked list
// 1-Wire driver stores info in file for device as text
struct ds18b20 {
	char 	devPath[128];
	char 	devID[16];
	char 	tempData[6];
	struct 	ds18b20 *next;
};




#endif
