CC=gcc
CFLAGS= -c -I/usr/include/mysql -L/usr/lib/mysql 
BUILD=.build

all: smart_sens

smart_sens: main.o mysql.o
	$(CC) $(BUILD)/main.o $(BUILD)/mysql.o -o $(BUILD)/smart_sens -lmysqlclient

main.o:
	$(CC) $(CFLAGS) main.c -o $(BUILD)/main.o

mysql.o:
	$(CC) $(CFLAGS) mysql.c -o $(BUILD)/mysql.o


clean:
	rm -rf $(BUILD)/*.o $(BUILD)/smart_sens