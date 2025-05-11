CC = clang
CFLAGS = -Wall -Wextra -std=c11

main: main.c lexer.o
	$(CC) -o main $(CFLAGS) main.c lexer.o

lexer.o: lexer.c lexer.h 
	$(CC) -c $(CFLAGS) lexer.c
