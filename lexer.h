#ifndef LEXEME_H
#define LEXEME_H
#include <stdio.h>

#include "lallocs.h"

typedef struct {
    char* file_name;
    int line;
    int col;
} Position;


typedef struct {
    Position *pos;
    char* value;
} Lexeme;

typedef struct {
    int size;
    int count;
    Lexeme* items;

} Lexemes;


Position* new_pos(List* ptrs, const char* const file_name, int line, int col);
void free_pos(List* ptrs, Position* pos);


void add_pos(List* ptrs, Lexeme* l, Position pos);
Lexeme* new_lexeme(List* ptrs, char* val, Position pos);

void print_lexeme(Lexeme lexeme);
void free_lexeme(List* ptrs,Lexeme* lexeme);

void new_darray(List* ptrs, Lexemes* arr);
int da_append(List *ptrs, Lexemes *arr, Lexeme item);
void free_lexemes(List* ptrs,Lexemes* arr);
void lex_file(List* ptrs, Lexemes* arr, FILE* file, const char* file_name);
#endif // LEXEME_H
