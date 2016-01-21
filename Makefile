CC=gcc

all: main

main:
	gcc main.c mysql.c -o .build/main -I/usr/include/mysql -L/usr/lib/mysql -lmysqlclient

clean:
	rm -rf *.o .build/main