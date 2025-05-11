CC = clang

main: main.c
	$(CC) -o main -Wall -Wextra main.c

