CC = gcc 
CFLAGS = -Wall -Wextra -ggdb

main: main.c 
	$(CC) -o main $(CFLAGS) main.c

