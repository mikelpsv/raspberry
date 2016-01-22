CC=gcc
CFLAGS= -c -Wall -I/usr/include/mysql -L/usr/lib/mysql 
BUILD=.build

all: smart_sens

smart_sens: main.o mysqlc.o
	$(CC) $(BUILD)/main.o $(BUILD)/mysqlc.o -o $(BUILD)/smart_sens -lmysqlclient

main.o:
	$(CC) $(CFLAGS) main.c -o $(BUILD)/main.o

mysqlc.o:
	$(CC) $(CFLAGS) mysqlc.c -o $(BUILD)/mysqlc.o


clean:
	rm -rf $(BUILD)/*.o $(BUILD)/smart_sens
