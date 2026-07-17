CC = gcc 
CFLAGS = -Wall -Wextra -ggdb

main: main.c tfidf.o 
	$(CC) -o main $(CFLAGS) main.c tfidf.o

tfidf.o: tfidf.c tfidf.h
	$(CC) -c $(CFLAGS) tfidf.c


