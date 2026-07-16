CC = gcc 
CFLAGS = -Wall -Wextra -std=c11 -ggdb

main: main.c 
	$(CC) -o main $(CFLAGS) main.c

