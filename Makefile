CC = clang
CFLAGS = -Wall -Wextra -std=c11

main: main.c 
	$(CC) -o main $(CFLAGS) main.c

